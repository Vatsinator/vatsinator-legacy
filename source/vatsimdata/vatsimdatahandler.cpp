/*
    vatsimdatahandler.cpp
    Copyright (C) 2012-2015  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <QtCore>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "events/vatsimevent.h"
#include "network/plaintextdownloader.h"
#include "plugins/bookingprovider.h"
#include "plugins/notamprovider.h"
#include "plugins/weatherforecastinterface.h"
#include "models/atctablemodel.h"
#include "models/metarlistmodel.h"
#include "ui/widgetsuserinterface.h"
#include "storage/cachefile.h"
#include "storage/settingsmanager.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/tma.h"
#include "vatsimdata/uir.h"
#include "vatsimdata/updatescheduler.h"
#include "vatsimdata/vatsimdatadocument.h"
#include "vatsimdata/vatsimstatusdocument.h"
#include "storage/filemanager.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "vatsimdatahandler.h"

/**
 * Name of the file in the cache directory that stores the recently
 * downloaded data file.
 */
static const QString CacheFileName = QStringLiteral("lastdata");

namespace {
QMap<QString, QString> countries; // used by __readCountryFile() and __readFirFile()

template <typename T>
T* selectPlugin()
{
    QString iid(qobject_interface_iid<T*>());
    auto plugins = QPluginLoader::staticPlugins();
    
    for (auto& p : plugins) {
        QJsonObject pluginData = p.metaData();
        
        if (!pluginData["IID"].isString())
            continue;
            
        QString iid = pluginData["IID"].toString();
        
        if (iid != iid)
            continue;
            
        T* i = qobject_cast<T*>(p.instance());
        
        if (i)
            return i;
    }
    
    return nullptr;
}
}


VatsimDataHandler::VatsimDataHandler(QObject* parent) :
    QObject(parent),
    __flights(new FlightTableModel(this)),
    __atcs(new AtcTableModel(this)),
    __status(NetConfig::Vatsim::statusUrl()),
    __currentTimestamp(0),
    __observers(0),
    __clientCount(0),
    __statusFileFetched(false),
    __initialized(false),
    __downloader(new PlainTextDownloader(this)),
    __scheduler(new UpdateScheduler(this))
{
    connect(__downloader, &PlainTextDownloader::finished, this, &VatsimDataHandler::__dataFetched);
    connect(__scheduler, &UpdateScheduler::timeToUpdate, this, &VatsimDataHandler::requestDataUpdate);
    
    connect(vApp()->userInterface(), &UserInterface::initialized, [this]() {
        if (isInitialized())
            __loadCachedData();
        else
            connect(this, &VatsimDataHandler::initialized, this, &VatsimDataHandler::__loadCachedData);
            
        /* The first download */
        __beginDownload();
    });
    
    connect(__downloader, &PlainTextDownloader::error, [this](QString error) {
        qWarning("Error downloading VATSIM data (%s)", qPrintable(error));
        
        /* Handle errors on download gracefully */
        if (__statusFileFetched) {
            emit vatsimDataError();
        } else {
            if (__status != NetConfig::Vatsim::backupStatusUrl()) {
                /* Try the backup url */
                __status = NetConfig::Vatsim::backupStatusUrl();
                __beginDownload();
            } else {
                /* We already tried - there is something else causing the error */
                emit vatsimStatusError();
            }
        }
    });
    
    connect(this, &VatsimDataHandler::vatsimDataDownloading, [this]() {
        __downloader->fetch(dataUrl());
    });
    
    
    /* Post events to the user interface */
    connect(this, &VatsimDataHandler::vatsimStatusError, [this]() {
        qApp->postEvent(vApp()->userInterface(), new VatsimEvent(VatsimEvent::StatusError));
    });
    
    connect(this, &VatsimDataHandler::vatsimDataError, [this]() {
        qApp->postEvent(vApp()->userInterface(), new VatsimEvent(VatsimEvent::DataError));
    });
}

VatsimDataHandler::~VatsimDataHandler()
{
    qDeleteAll(__clients);
    qDeleteAll(__airports);
    qDeleteAll(__firs);
}

void
VatsimDataHandler::initialize()
{
    __initializeData();
    
    __readCountryFile(FileManager::path("data/country"));
    __readAliasFile(FileManager::path("data/alias"));
    __readFirFile(FileManager::path("data/fir"));
    __readUirFile(FileManager::path("data/uir"));
    __readTmaFile(FileManager::path("data/tma"));
    
    __initialized = true;
    emit initialized();
}

const QUrl&
VatsimDataHandler::dataUrl() const
{
    if (__statusFileFetched) {
        qsrand(QTime::currentTime().msec());
        return __dataServers.at(qrand() % __dataServers.size());
    } else
        return __status;
}

Airport*
VatsimDataHandler::findAirport(const QString& icao)
{
    if (__airports.contains(icao))
        return __airports[icao];
    else {
        QList<QString> keys = __aliases.values(icao);
        
        if (icao.length() < 4) /* Handle USA aliases (JFK -> KJFK) */
            keys.prepend(QStringLiteral("K") % icao);
            
        for (const QString& k : keys) {
            if (__airports.contains(k))
                return __airports[k];
        }
        
        return nullptr;
    }
}

QList<Client*>
VatsimDataHandler::clients() const
{
    return __clients.values();
}

QList<Airport*>
VatsimDataHandler::airports() const
{
    return __airports.values();
}

Fir*
VatsimDataHandler::findFir(const QString& icao, bool fss)
{
    QList<QString> keys = __aliases.values(icao);
    
    if (icao.length() >= 4)
        keys.prepend(icao);
    else
        /* Handle USA 3-letter icao contraction */
        keys.prepend(QString("K") % icao);
        
    for (const QString& k : keys) {
        if (__firs.contains(k)) {
            QList<Fir*> values = __firs.values(k);
            
            for (Fir* f : values) {
                if (f->isOceanic() == fss && f->hasValidPosition())
                    return f;
            }
        }
    }
    
    return nullptr;
}

Uir*
VatsimDataHandler::findUir(const QString& icao)
{
    if (__uirs.contains(icao))
        return __uirs[icao];
        
    QList<QString> keys = __aliases.values(icao);
    
    for (const QString& k : keys) {
        if (__uirs.contains(k))
            return __uirs[k];
    }
    
    return nullptr;
}

Tma*
VatsimDataHandler::findTma(const QString& icao)
{
    return __tmas.value(icao, nullptr);
}

QString
VatsimDataHandler::alternameName(const QString& icao)
{
    if (!__alternameNames.contains(icao))
        return QString();
    else
        return __alternameNames[icao];
}

QList<Fir*>
VatsimDataHandler::firs() const
{
    return qMove(__firs.values());
}

QList<Uir*>
VatsimDataHandler::uirs() const
{
    return qMove(__uirs.values());
}

int
VatsimDataHandler::clientCount() const
{
    return __clientCount;
}

int
VatsimDataHandler::pilotCount() const
{
    int p = 0;
    
    for (Client* c : __clients.values()) {
        if (Pilot* pp = qobject_cast<Pilot*>(c)) {
            if (!pp->isPrefiledOnly())
                p += 1;
        }
    }
    
    return p;
}

int
VatsimDataHandler::atcCount() const
{
    int cc = 0;
    
    for (Client* c : __clients.values()) {
        if (qobject_cast<Controller*>(c))
            cc += 1;
    }
    
    return cc;
}

int
VatsimDataHandler::obsCount() const
{
    return __observers;
}

NotamProvider*
VatsimDataHandler::notamProvider()
{
    static NotamProvider* provider = selectPlugin<NotamProvider>();
    return provider;
}

BookingProvider*
VatsimDataHandler::bookingProvider()
{
    static BookingProvider* provider = selectPlugin<BookingProvider>();
    return provider;
}

qreal
VatsimDataHandler::fastDistance(
    const qreal& lat1, const qreal& lon1,
    const qreal& lat2, const qreal& lon2)
{
    return qSqrt(
               qPow(lat1 - lat2, 2) +
               qPow(lon1 - lon2, 2)
           );
}

qreal
VatsimDataHandler::fastDistance(const LonLat& a, const LonLat& b)
{
    return qSqrt(
               qPow(a.latitude() - b.latitude(), 2) +
               qPow(a.longitude() - b.longitude(), 2)
           );
}

qreal
VatsimDataHandler::nmDistance(
    const qreal& lat1, const qreal& lon1,
    const qreal& lat2, const qreal& lon2)
{
    return nmDistance(LonLat(lon1, lat1), LonLat(lon2, lat2));
}

qreal
VatsimDataHandler::nmDistance(const LonLat& a, const LonLat& b)
{
    /* http://www.movable-type.co.uk/scripts/latlong.html */
    static Q_CONSTEXPR qreal R = 3440.06479191; // nm
    
    /* Equirectangular approximation */
    qreal x = (qDegreesToRadians(b.longitude() - a.longitude())) *
        qCos((qDegreesToRadians(a.latitude()) + qDegreesToRadians(b.latitude())) / 2);
    
    qreal y = qDegreesToRadians(b.latitude()) - qDegreesToRadians(a.latitude());
    
    return qSqrt(x * x + y * y) * R;
}

bool
VatsimDataHandler::isValidIcao(const QString& str)
{
    QRegExp rx("[a-zA-z0-9]{4}");
    return rx.exactMatch(str);
}

void
VatsimDataHandler::requestDataUpdate()
{
    if (__downloader->isWorking())
        __downloader->abort();
        
    emit vatsimDataDownloading();
}

void
VatsimDataHandler::__readAliasFile(const QString& fileName)
{
    qDebug("Reading %s...", qPrintable(fileName));
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        notifyWarning(tr("File %1 could not be opened. Please reinstall the application.").arg(file.fileName()));
        return;
    }
    
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning("VatsimDataHandler: the following error occured parsing %s: %s",
                 qPrintable(file.fileName()), qPrintable(error.errorString()));
        notifyWarning(tr("File %1 could not be read. Please reinstall the application.").arg(file.fileName()));
        return;
    }
    
    Q_ASSERT(document.isArray());
    QJsonArray array = document.array();
    
    /* The ugliest loop I ever made */
    for (const QJsonValueRef a : array) {
        QJsonObject object = a.toObject();
        QString target = object["target"].toString();
        QJsonArray aliasArray = object["alias"].toArray();
        
        for (const QJsonValueRef b : aliasArray) {
            if (b.isString())
                __aliases.insert(target, b.toString());
            else if (b.isObject()) {
                QJsonObject aliasObject = b.toObject();
                QString name = aliasObject["name"].toString();
                
                QJsonArray anotherArray = object["alias"].toArray();
                
                for (const QJsonValueRef c : anotherArray) {
                    __aliases.insert(target, c.toString());
                    __alternameNames.insert(c.toString(), name);
                }
            }
        }
    }
    
    file.close();
}

void
VatsimDataHandler::__readCountryFile(const QString& fileName)
{
    qDebug("Reading %s...", qPrintable(fileName));
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        notifyWarning(tr("File %1 could not be opened. Please reinstall the application.").arg(fileName));
        return;
    }
    
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning("VatsimDataHandler: the following error occured parsing %s: %s",
                 qPrintable(file.fileName()), qPrintable(error.errorString()));
        notifyWarning(tr("File %1 could not be read. Please reinstall the application.").arg(file.fileName()));
        return;
    }
    
    Q_ASSERT(document.isArray());
    QJsonArray array = document.array();
    
    for (const QJsonValueRef c : array) {
        QJsonObject object = c.toObject();
        QString country = object["country"].toString();
        QString code = object["code"].toString();
        countries.insert(code, country);
    }
    
    file.close();
}

void
VatsimDataHandler::__readFirFile(const QString& fileName)
{
    qDebug("Reading %s...", qPrintable(fileName));
    
    QFile file(fileName);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        notifyWarning(tr("File %1 could not be opened. Please reinstall the application.").arg(fileName));
        return;
    }
    
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning("VatsimDataHandler: the following error occured parsing %s: %s",
                 qPrintable(file.fileName()), qPrintable(error.errorString()));
        notifyWarning(tr("File %1 could not be read. Please reinstall the application.").arg(file.fileName()));
        return;
    }
    
    Q_ASSERT(document.isArray());
    QJsonArray array = document.array();
    
    for (const QJsonValueRef c : array) {
        QJsonObject object = c.toObject();
        QString icao = object["icao"].toString();
        QString name = object["name"].toString();
        
        Fir* currentFir = findFir(icao);
        
        if (currentFir) {
            currentFir->setName(name);
            
            /*
             * TODO Make this below more convenient, i.e. allow writing some kind of
             *      "exceptions" in the data file.
             */
            if (currentFir->icao() == "UMKK") // fix for Kaliningrad center
                currentFir->setCountry("Russia");
            else
                currentFir->setCountry(countries[icao.left(2)]);
                
            currentFir->fixupName();
        }
        
        // look for some oceanic fir
        currentFir = findFir(icao, true);
        
        if (currentFir) {
            currentFir->setName(name);
            currentFir->setCountry(countries[icao.left(2)]);
            currentFir->fixupName();
        }
    }
    
    file.close();
}

void
VatsimDataHandler::__readUirFile(const QString& fileName)
{
    qDebug("Reading %s...", qPrintable(fileName));
    
    QFile file(fileName);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        notifyWarning(tr("File %1 could not be opened. Please reinstall the application.").arg(fileName));
        return;
    }
    
    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine()).simplified();
        
        if (line[0] == '#' || line.isEmpty())
            continue;
            
        QStringList data = line.split(' ');
        Uir* uir = new Uir(data.first());
        
        for (int i = 1; i < data.length(); ++i) {
            if (data[i].toUpper() == data[i]) {
                Fir* fir = findFir(data[i]);
                
                if (fir)
                    uir->addFir(fir);
            } else
                uir->name().append(data[i] + " ");
        }
        
        __uirs.insert(uir->icao(), uir);
    }
    
    file.close();
}

void
VatsimDataHandler::__readTmaFile(const QString& fileName)
{
    qDebug("Reading %s...", qPrintable(fileName));
    
    QFile file(fileName);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        notifyWarning(tr("File %1 could not be opened. Please reinstall the application.").arg(fileName));
        return;
    }
    
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning("VatsimDataHandler: the following error occured parsing %s: %s",
                 qPrintable(file.fileName()), qPrintable(error.errorString()));
        notifyWarning(tr("File %1 could not be read. Please reinstall the application.").arg(file.fileName()));
        return;
    }
    
    Q_ASSERT(document.isObject());
    QJsonObject object = document.object();
    
    auto keys = object.keys();
    std::for_each(keys.begin(), keys.end(), [this, &object](const QString & key) {
        auto value = object.value(key);
        Q_ASSERT(value.isArray());;
        auto array = value.toArray();
        Tma* tma = new Tma(key, array, this);
        __tmas.insert(key, tma);
    });
    
    file.close();
}

void
VatsimDataHandler::__initializeData()
{
    std::for_each(vApp()->firDatabase()->firs().begin(),
                  vApp()->firDatabase()->firs().end(),
        [this](const FirRecord & fr) {
            Fir* f = new Fir(&fr);
            __firs.insert(f->icao(), f);
        }
    );
                 
    std::for_each(vApp()->airportDatabase()->airports().begin(),
                  vApp()->airportDatabase()->airports().end(),
        [this](const AirportRecord& ar) {
            /* TODO Remove invalid ICAOs from the database itself */
            if (!isValidIcao(ar.icao)) {
                qWarning("Invalid airport icao in the database: %s", ar.icao);
                return;
            }
            
            Airport* a = new Airport(&ar);
            __airports.insert(a->icao(), a);
        }
    );
}

void
VatsimDataHandler::__parseDataDocument(const QByteArray& data, bool* ok)
{
    VatsimDataDocument document(data);
    
    if (!document.isValid()) {
        *ok = false;
        return;
    }
    
    __dateVatsimDataUpdated = document.update();
    __reload = document.reload();
    __clientCount = document.connectedClients();
    
    /* Update timestamp to keep track of clients that logged off */
    __currentTimestamp = QDateTime::currentMSecsSinceEpoch();
    __newClients.clear();
    __observers = 0;
    
    for (auto& c : document.clients()) {
        if (c.type == VatsimDataDocument::ClientLine::Atc) {
            /* Multiple PIDs for ATCs allowed, unique key is pid + callsign */
            auto it = __clients.find(c.pid);
            while (it != __clients.end() && it.value()->pid() == c.pid && it.value()->callsign() != c.callsign)
                ++it;
            
            if (it != __clients.end() && it.value()->pid() == c.pid) {
                it.value()->update(c.line);
                continue;
            }
            
            /* No such ATC with the given callsign found */
            Controller* atc = new Controller(c.line);
            
            if (atc->isValid()) {
                __clients.insertMulti(atc->pid(), atc);
                __newClients << atc;
                __atcs->add(atc);
            } else {
                __observers += 1;
                atc->deleteLater();
            }
        } else { // Pilot
            /* Multiple PIDs for pilots not allowed */
            if (__clients.count(c.pid) > 1) {
                qWarning("Multiple PIDs (%d) for client", c.pid);
                __clients.remove(c.pid);
            }
            
            Q_ASSERT(__clients.count(c.pid) <= 1);
            if (__clients.contains(c.pid)) {
                __clients[c.pid]->update(c.line);
                continue;
            }
            
            Pilot* pilot = new Pilot(c.line);
            
            if (pilot->position().isNull())
                pilot->deleteLater();
            else {
                __clients[pilot->pid()] = pilot;
                __newClients << pilot;
                __flights->add(pilot);
            }
        }
    }
    
    for (auto& p : document.prefile()) {
        if (p.type == VatsimDataDocument::ClientLine::Atc)
            continue;
            
        if (!__clients.contains(p.pid)) {
            Pilot* pilot = new Pilot(p.line, true);
            __clients[pilot->pid()] = pilot;
        }
    }
    
    __cleanupClients();
    *ok = true;
}

void
VatsimDataHandler::__cleanupClients()
{
    qDeleteAll(__invalidClients);
    __invalidClients.clear();
    
    for (auto it = __clients.begin(); it != __clients.end();) {
        if (!it.value()->isOnline()) {
            Client* c = it.value();
            c->invalidate();
            it = __clients.erase(it);
            __invalidClients << c;
        } else
            ++it;
    }
}

void
VatsimDataHandler::__loadCachedData()
{
    qDebug("VatsimDataHandler: loading data from cache...");
    
    CacheFile file(CacheFileName);
    
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning("VatsimDataHandler: cache file %s is not readable.",
                     qPrintable(file.fileName()));
            return;
        }
        
        QString data(file.readAll());
        file.close();
        //     parseDataFile(data);
    }
    
    qDebug("VatsimDataHandler: cache restored.");
}

void
VatsimDataHandler::__beginDownload()
{
    qDebug("VatsimDataHandler: starting download");
    __downloader->fetch(dataUrl());
}

void
VatsimDataHandler::__dataFetched()
{
    if (__statusFileFetched) {
        bool ok;
        __parseDataDocument(__downloader->data(), &ok);
        
        if (ok) {
            emit vatsimDataUpdated();
            FileManager::cacheData(CacheFileName, __downloader->data());
        } else
            emit vatsimDataError();
    } else {
        VatsimStatusDocument status(__downloader->data());
        
        if (!status.isValid())
            emit vatsimStatusError();
        else if (!status.moveTo().isEmpty()) {
            __status = status.moveTo();
            __beginDownload();
        } else {
            __metar = status.metar();
            __dataServers = status.dataFileUrls();
            
            if (!status.message().isEmpty()) {
                qApp->postEvent(vApp()->userInterface(), new VatsimEvent(VatsimEvent::Message, status.message()));
            }
                
            __statusFileFetched = true;
            emit vatsimStatusUpdated();
            
            /* Start downloading data when status document is read */
            emit vatsimDataDownloading();
        }
    }
}
