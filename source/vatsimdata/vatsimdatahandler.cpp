/*
    vatsimdatahandler.cpp
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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
#include "events/decisionevent.h"
#include "network/abstractnotamprovider.h"
#include "network/euroutenotamprovider.h"
#include "network/plaintextdownloader.h"
#include "plugins/weatherforecastinterface.h"
#include "modules/modulemanager.h"
#include "ui/pages/miscellaneouspage.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/userinterface.h"
#include "ui/widgetsuserinterface.h"
#include "storage/cachefile.h"
#include "storage/settingsmanager.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/uir.h"
#include "vatsimdata/updatescheduler.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsimdata/models/metarlistmodel.h"
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
}


VatsimDataHandler::VatsimDataHandler(QObject* parent) :
    QObject(parent),
    __statusUrl(NetConfig::Vatsim::statusUrl()),
    __currentTimestamp(0),
    __observers(0),
    __clientCount(0),
    __statusFileFetched(false),
    __initialized(false),
    __downloader(new PlainTextDownloader()),
    __scheduler(new UpdateScheduler(this)),
    __notamProvider(nullptr),
    __weatherForecast(nullptr) {
  
  connect(vApp()->userInterface(),              SIGNAL(initialized()),
          this,                                 SLOT(__slotUiCreated()));
  connect(__downloader,                         SIGNAL(finished(QString)),
          this,                                 SLOT(__dataFetched(QString)));
  connect(__downloader,                         SIGNAL(error()),
          this,                                 SLOT(__handleFetchError()));
  connect(__scheduler,                          SIGNAL(timeToUpdate()),
          this,                                 SLOT(requestDataUpdate()));
  connect(this,                                 SIGNAL(vatsimStatusError()),
          vApp()->userInterface(),              SLOT(statusError()));
  connect(this,                                 SIGNAL(vatsimDataError()),
          vApp()->userInterface(),              SLOT(dataError()));
  connect(vApp()->settingsManager(),            SIGNAL(settingsChanged()),
          this,                                 SLOT(__reloadWeatherForecast()));
  
  connect(this, SIGNAL(vatsimDataDownloading()), SLOT(__beginDownload()));
  
  __notamProvider = new EurouteNotamProvider(this);
}

VatsimDataHandler::~VatsimDataHandler() {
  qDeleteAll(__clients);
  qDeleteAll(__airports);
  qDeleteAll(__firs);
  
  delete __downloader;
}

void
VatsimDataHandler::initialize() {
  __initializeData();
  
  __readCountryFile(FileManager::path("data/country"));
  __readAliasFile(FileManager::path("data/alias"));
  __readFirFile(FileManager::path("data/fir"));
  __readUirFile(FileManager::path("data/uir"));
  
  __initialized = true;
  emit initialized();
}

void
VatsimDataHandler::parseStatusFile(const QString& content) {
  static QRegExp rx("(msg0|url0|moveto0|metar0)=(.+)\\b");
  
  QStringList tempList = content.split('\n', QString::SkipEmptyParts);
  
  for (QString& temp: tempList) {
    if (temp.startsWith(';'))
      continue;
    
    if (rx.indexIn(temp) >= 0) {
      QString key = rx.cap(1);
      QString value = rx.cap(2);
      
      if (key == "moveto0") {
        __statusUrl = value;
        __beginDownload();
        return;
      } else if (key == "metar0") {
        __metarUrl = value;
      } else if (key == "url0") {
        __dataServers << value;
      } else if (key == "msg0") {
        vApp()->userInterface()->showVatsimMessage(value);
      }
    }
  }
  
  if (__metarUrl.isEmpty() || __dataServers.empty()) {
    emit vatsimStatusError();
  } else {
    __statusFileFetched = true;
    emit vatsimStatusUpdated();
    emit vatsimDataDownloading();
  }
}

void
VatsimDataHandler::parseDataFile(const QString& content) {
  static QRegExp rx("^\\b(UPDATE|RELOAD|CONNECTED CLIENTS)\\b\\s?=\\s?\\b(.+)\\b$");

  qDebug("Data length: %i.", content.length());

  QStringList tempList = content.split(QRegExp("\r?\n"), QString::SkipEmptyParts);  
  DataSections section = None;
  
  __currentTimestamp = QDateTime::currentMSecsSinceEpoch();
  __newClients.clear();
  __observers = 0;

  for (QString& temp: tempList) {
    if (temp.startsWith(';')) // comment
      continue;

    if (temp.startsWith('!')) {
      section = None;
      
      if (temp.contains("GENERAL"))
        section = DataSections::General;
      else if (temp.contains("CLIENTS"))
        section = DataSections::Clients;
      else if (temp.contains("PREFILE"))
        section = DataSections::Prefile;

      continue;
    }
    
    switch (section) {
      case DataSections::General: {
        if (rx.indexIn(temp) >= 0) {
          QString key = rx.cap(1);
          QString value = rx.cap(2);
          
          if (key == "UPDATE") {
            __dateVatsimDataUpdated = QDateTime::fromString(
              value, "yyyyMMddhhmmss");
          } else if (key == "RELOAD") {
            __reload = value.toInt();
          } else if (key == "CONNECTED CLIENTS") {
            __clientCount = value.toInt();
          }
        }
        break;
      } // DataSections::General
    
      case DataSections::Clients: {
        RawClientData clientData = RawClientData(temp);
        
        if (!clientData.valid)
          continue;
        
        if (__clients.contains(clientData.callsign)) {
          Client* c = __clients[clientData.callsign];
          c->update(clientData.line);
        } else {
          if (clientData.type == RawClientData::Atc) {
            Controller* atc = new Controller(clientData.line);
            
            if (atc->isOk()) {
              __clients[atc->callsign()] = atc;
              __newClients << atc;
            } else {
              __observers += 1;
              delete atc;
            }
          } else if (clientData.type == RawClientData::Pilot) {
            Pilot* pilot = new Pilot(clientData.line);
            if (pilot->position().isNull()) {
              delete pilot; // skip unknown flights
            } else {
              __clients[pilot->callsign()] = pilot;
              __newClients << pilot;
            }
          }
        }
        break;
      } // DataSections::Clients
    
      case DataSections::Prefile: {
        RawClientData clientData = RawClientData(temp);
        
        if (!clientData.valid || clientData.type != RawClientData::Pilot)
          continue;
        
        if (!__clients.contains(clientData.callsign)) {
          Pilot* pilot = new Pilot(clientData.line, true);
          __clients[pilot->callsign()] = pilot;
        }
        
        break;
      } // DataSections::Prefile
      
      default: {}
    } // switch (section)
  } // for
  
  __cleanupClients();
}

const QString&
VatsimDataHandler::getDataUrl() const {
  if (__statusFileFetched) {
    qsrand(QTime::currentTime().msec());
    return __dataServers[qrand() % __dataServers.size()];
  } else {
    return __statusUrl;
  }
}

FlightTableModel*
VatsimDataHandler::flightTableModel() const {
  FlightTableModel* model = new FlightTableModel();
  for (Client* c: __clients.values()) {
    if (Pilot* p = dynamic_cast<Pilot*>(c))
      model->add(p);
  }
  
  return model;
}

ControllerTableModel*
VatsimDataHandler::controllerTableModel() const {
  ControllerTableModel* model = new ControllerTableModel();
  for (Client* c: __clients.values()) {
    if (Controller* cc = dynamic_cast<Controller*>(c))
      model->add(cc);
  }
  
  return model;
}

const Pilot*
VatsimDataHandler::findPilot(const QString& callsign) const {
  if (__clients.contains(callsign))
    return dynamic_cast<Pilot*>(__clients[callsign]);
  else
    return nullptr;
}

const Controller*
VatsimDataHandler::findAtc(const QString& callsign) const {
  if (__clients.contains(callsign))
    return dynamic_cast<Controller*>(__clients[callsign]);
  else
    return nullptr;
}

Airport*
VatsimDataHandler::findAirport(const QString& icao) {
  if (__airports.contains(icao)) {
    return __airports[icao];
  } else {
    QList<QString> keys = __aliases.values(icao);
    if (icao.length() < 4)
      keys.prepend(QString("K") % icao);
    
    for (const QString& k: keys) {
      if (__airports.contains(k))
        return __airports[k];
    }
    
    return nullptr;
  }
}

QList<Airport*>
VatsimDataHandler::airports() const {
  return qMove(__airports.values());
}

Fir*
VatsimDataHandler::findFir(const QString& icao, bool fss) {
  QList<QString> keys = __aliases.values(icao);
  if (icao.length() >= 4)
    keys.prepend(icao);
  else
    /* Handle USA 3-letter icao contraction */
    keys.prepend(QString("K") % icao);
  
  for (const QString& k: keys) {
    if (__firs.contains(k)) {
      QList<Fir*> values = __firs.values(k);
      for (Fir* f: values) {
        if (f->isOceanic() == fss && f->hasValidPosition()) {
          return f;
        }
      }
    }
  }
  
  return nullptr;
}

Uir*
VatsimDataHandler::findUir(const QString& icao) {
  if (__uirs.contains(icao))
    return __uirs[icao];
  
  QList<QString> keys = __aliases.values(icao);
  for (const QString& k: keys) {
    if (__uirs.contains(k))
      return __uirs[k];
  }
  
  return nullptr;
}

QString
VatsimDataHandler::alternameName(const QString& icao) {
  if (!__alternameNames.contains(icao))
    return QString();
  else
    return __alternameNames[icao];
}

QList<Fir*>
VatsimDataHandler::firs() const {
  return qMove(__firs.values());
}

QList<Uir*>
VatsimDataHandler::uirs() const {
  return qMove(__uirs.values());
}

int
VatsimDataHandler::clientCount() const {
  return __clientCount;
}

int
VatsimDataHandler::pilotCount() const {
  int p = 0;
  for (Client* c: __clients.values()) {
    if (Pilot* pp = dynamic_cast<Pilot*>(c)) {
      if (!pp->isPrefiledOnly())
        p += 1;
    }
  }
  
  return p;
}

int
VatsimDataHandler::atcCount() const {
  int cc = 0;
  for (Client* c: __clients.values()) {
    if (dynamic_cast<Controller*>(c))
      cc += 1;
  }
  
  return cc;
}

int
VatsimDataHandler::obsCount() const {
  return __observers;
}

AbstractNotamProvider*
VatsimDataHandler::notamProvider() {
  if (__notamProvider == nullptr)
    __notamProvider = new EurouteNotamProvider();
  
  return __notamProvider;
}

WeatherForecastInterface*
VatsimDataHandler::weatherForecastProvider() {
  return __weatherForecast;
}

bool
VatsimDataHandler::event(QEvent* _event) {
  if (_event->type() == Event::Decision) {
    userDecisionEvent(static_cast<DecisionEvent*>(_event));
    return true;
  } else {
    return QObject::event(_event);
  }
}

qreal
VatsimDataHandler::fastDistance(
    const qreal& lat1, const qreal& lon1,
    const qreal& lat2, const qreal& lon2) {
  return qSqrt(
    qPow(lat1 - lat2, 2) +
    qPow(lon1 - lon2, 2)
  );
}

qreal
VatsimDataHandler::fastDistance(const LonLat& a, const LonLat& b) {
  return qSqrt(
    qPow(a.latitude() - b.latitude(), 2) +
    qPow(a.longitude() - b.longitude(), 2)
  );
}

qreal
VatsimDataHandler::nmDistance(
    const qreal& lat1, const qreal& lon1,
    const qreal& lat2, const qreal& lon2) {
  
  /* http://www.movable-type.co.uk/scripts/latlong.html */
  static Q_DECL_CONSTEXPR qreal R = 3440.06479191; // nm
  
  return qAcos(
      qSin(lat1) * qSin(lat2) +
      qCos(lat1) * qCos(lat2) *
      qCos(lon2 - lon1)
    ) * R;
}

qreal
VatsimDataHandler::nmDistance(const LonLat& a, const LonLat& b) {
  /* http://www.movable-type.co.uk/scripts/latlong.html */
  static Q_DECL_CONSTEXPR qreal R = 3440.06479191; // nm
  
  return qAcos(
      qSin(a.latitude()) * qSin(b.latitude()) +
      qCos(a.latitude()) * qCos(b.latitude()) *
      qCos(b.longitude() - a.longitude())
    ) * R;
}

void
VatsimDataHandler::requestDataUpdate() {
  if (__downloader->isWorking())
    __downloader->abort();
  
  emit vatsimDataDownloading();
}

void
VatsimDataHandler::userDecisionEvent(DecisionEvent* event) {
  if (event->context() == QStringLiteral("data_fetch_error") &&
      event->decision() == DecisionEvent::TryAgain) {
    requestDataUpdate();
  }
}

void
VatsimDataHandler::__readAliasFile(const QString& fileName) {
  qDebug("Reading \"alias\" file...");
  
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
  for (const QJsonValueRef a: array) {
    QJsonObject object = a.toObject();
    QString target = object["target"].toString();
    QJsonArray aliasArray = object["alias"].toArray();
    for (const QJsonValueRef b: aliasArray) {
      if (b.isString()) {
        __aliases.insert(target, b.toString());
      } else if (b.isObject()) {
        QJsonObject aliasObject = b.toObject();
        QString name = aliasObject["name"].toString();
        
        QJsonArray anotherArray = object["alias"].toArray();
        for (const QJsonValueRef c: anotherArray) {
          __aliases.insert(target, c.toString());
          __alternameNames.insert(c.toString(), name);
        }
      }
    }
  }
  
  file.close();
  
  qDebug("Finished reading \"alias\" file.");
}

void
VatsimDataHandler::__readCountryFile(const QString& fileName) {
  qDebug("Reading \"country\" file...");
  
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
  for (const QJsonValueRef c: array) {
    QJsonObject object = c.toObject();
    QString country = object["country"].toString();
    QString code = object["code"].toString();
    countries.insert(code, country);
  }
  
  file.close();
  
  qDebug("Finished reading \"country\" file.");
}

void
VatsimDataHandler::__readFirFile(const QString& fileName) {
  qDebug("Reading \"fir\" file...");
  
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
  
  for (const QJsonValueRef c: array) {
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
  
  qDebug("Finished reading \"fir\" file.");
}

void
VatsimDataHandler::__readUirFile(const QString& fileName) {
  qDebug("Reading \"uir\" file...");
  
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
      } else {
        uir->name().append(data[i] + " ");
      }
    }
    
    __uirs.insert(uir->icao(), uir);
  }
  
  file.close();
  
  qDebug("Finished reading \"uir\" file.");
}

void
VatsimDataHandler::__initializeData() {
  std::for_each(vApp()->firDatabase()->firs().begin(),
                vApp()->firDatabase()->firs().end(),
                [this](const FirRecord& fr) {
      Fir* f = new Fir(&fr);
      __firs.insert(f->icao(), f);
    }
  );
  
  std::for_each(vApp()->airportDatabase()->airports().begin(),
                vApp()->airportDatabase()->airports().end(),
                [this](const AirportRecord& ar) {
      Airport* a = new Airport(&ar);
      __airports.insert(a->icao(), a);
    }
  );
}

void
VatsimDataHandler::__cleanupClients() {
  qDeleteAll(__invalidClients);
  __invalidClients.clear();
  
  for (auto it = __clients.begin(); it != __clients.end();) {
    if (!it.value()->isOnline()) {
      Client* c = it.value();
      c->invalidate();
      it = __clients.erase(it);
      __invalidClients << c;
    } else {
      ++it;
    }
  }
}

void
VatsimDataHandler::__loadCachedData() {
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
    parseDataFile(data);
  }
  
  qDebug("VatsimDataHandler: cache restored.");
}

void
VatsimDataHandler::__slotUiCreated() {
  bool cacheEnabled = SM::get("network.cache_enabled").toBool();
  if (cacheEnabled && isInitialized())
    __loadCachedData();
  else if (cacheEnabled)
    connect(this, &VatsimDataHandler::initialized, this, &VatsimDataHandler::__loadCachedData);
  
  /* TODO Move the below to UserInterface */
  if (wui())
    __downloader->setProgressBar(wui()->mainWindow()->progressBar());
  
  /* The first download */
  __beginDownload();
}

void
VatsimDataHandler::__beginDownload() {
  qDebug("VatsimDataHandler: starting download.");
  __downloader->fetch(getDataUrl());
}

void
VatsimDataHandler::__dataFetched(QString data) {
  if (__statusFileFetched) {
    if (data.isEmpty()) {
      emit vatsimDataError();
      return;
    }
    
    parseDataFile(data);
    emit vatsimDataUpdated();
    
    if (SM::get("network.cache_enabled").toBool())
      FileManager::cacheData(CacheFileName, data);
    
    MetarListModel::getSingleton().updateAll();
  } else {
    parseStatusFile(data);
  }
}

void
VatsimDataHandler::__handleFetchError() {
  if (__statusFileFetched) {
    emit vatsimDataError();
  } else {
    if (__statusUrl != NetConfig::Vatsim::backupStatusUrl()) {
      /* Try the backup url */
      __statusUrl = NetConfig::Vatsim::backupStatusUrl();
      __beginDownload();
    } else {
      /* We already tried - there is something else causing the error */
      emit vatsimStatusError();
    }
  }
}

void
VatsimDataHandler::__reloadWeatherForecast() {
  QString desired = SM::get("network.weather_forecast_provider").toString();
  /* TODO Plugin selection needs to be done better  */
  if (desired != "none") {
    QDir pluginsDir(FileManager::staticPath(FileManager::Plugins));
    for (QString fileName: pluginsDir.entryList(QDir::Files)) {
      QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
      QJsonObject pluginData = loader.metaData();
      if (pluginData["IID"].toString() == "org.eu.vatsinator.Vatsinator.WeatherForecastInterface") {
        QJsonObject metaData = pluginData["MetaData"].toObject();
        QString name = metaData["name"].toString();
        
        if (desired == name) {
          __weatherForecast = qobject_cast<WeatherForecastInterface*>(loader.instance());
          Q_ASSERT(__weatherForecast);
          qDebug("Loaded weather forecast plugin: %s",
                 qPrintable(metaData["provider_name"].toString()));
          break;
        }
      }
    }
  } else {
    __weatherForecast = nullptr;
  }
}


VatsimDataHandler::RawClientData::RawClientData(const QString& _line) {
  line = _line.split(':');
  valid = line.size() == 42;
  if (valid) {
    callsign = line[0];
    type = line[3] == "ATC" ? Atc : Pilot;
  }
}
