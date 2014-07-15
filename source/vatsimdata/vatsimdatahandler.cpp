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
#include <QtGui>
#include <qjson/parser.h>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "network/abstractnotamprovider.h"
#include "network/euroutenotamprovider.h"
#include "network/plaintextdownloader.h"
#include "modules/modulemanager.h"
#include "ui/pages/miscellaneouspage.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/userinterface.h"
#include "storage/cachefile.h"
#include "storage/settingsmanager.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/uir.h"
#include "vatsimdata/updatescheduler.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsimdata/models/metarlistmodel.h"
#include "storage/filemanager.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "vatsimdatahandler.h"

using std::for_each;

static const QString CacheFileName = "lastdata";

FlightTableModel* VatsimDataHandler::emptyFlightTable = new FlightTableModel();
ControllerTableModel* VatsimDataHandler::emptyControllerTable = new ControllerTableModel();

static QMap<QString, QString> countries; // used by __readCountryFile() and __readFirFile()


VatsimDataHandler::VatsimDataHandler(QObject* _parent) :
    QObject(_parent),
    __statusUrl(NetConfig::Vatsim::statusUrl()),
    __currentTimestamp(0),
    __observers(0),
    __clientCount(0),
    __statusFileFetched(false),
    __initialized(false),
    __downloader(new PlainTextDownloader()),
    __scheduler(new UpdateScheduler()),
    __notamProvider(nullptr) {
  
  connect(VatsinatorApplication::getSingletonPtr(), SIGNAL(uiCreated()),
          this,                                     SLOT(__slotUiCreated()));
  connect(__downloader,                             SIGNAL(finished(QString)),
          this,                                     SLOT(__dataFetched(QString)));
  connect(__downloader,                             SIGNAL(error()),
          this,                                     SLOT(__handleFetchError()));
  connect(__scheduler,                              SIGNAL(timeToUpdate()),
          this,                                     SLOT(requestDataUpdate()));
  connect(this,                                     SIGNAL(localDataBad(QString)),
          UserInterface::getSingletonPtr(),         SLOT(warning(QString)));
  
  connect(this, SIGNAL(vatsimDataDownloading()), SLOT(__beginDownload()));
  
  __notamProvider = new EurouteNotamProvider();
}

VatsimDataHandler::~VatsimDataHandler() {
  __clearData();
  qDeleteAll(__airports);
  qDeleteAll(__firs);
  
  if (__notamProvider)
    delete __notamProvider;
  
  delete __downloader;
  delete __scheduler;
  
  delete VatsimDataHandler::emptyFlightTable;
  delete VatsimDataHandler::emptyControllerTable;
}

void
VatsimDataHandler::init() {
  __initData();
  
  __readCountryFile(FileManager::path("data/country"));
  __readAliasFile(FileManager::path("data/alias"));
  __readFirFile(FileManager::path("data/fir"));
  __readUirFile(FileManager::path("data/uir"));
  
  __initialized = true;
  emit initialized();
}

void
VatsimDataHandler::parseStatusFile(const QString& _statusFile) {
  static QRegExp rx("(msg0|url0|moveto0|metar0)=(.+)\\b");
  
  QStringList tempList = _statusFile.split('\n', QString::SkipEmptyParts);
  
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
        UserInterface::getSingleton().showVatsimMessage(value);
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
VatsimDataHandler::parseDataFile(const QString& _data) {
  static QRegExp rx("^\\b(UPDATE|RELOAD|CONNECTED CLIENTS)\\b\\s?=\\s?\\b(.+)\\b$");

  VatsinatorApplication::log("Data length: %i.", _data.length());

  QStringList tempList = _data.split(QRegExp("\r?\n"), QString::SkipEmptyParts);  
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
        
        if (__clients.contains(clientData.callsign))
          __clients[clientData.callsign]->deleteLater();
        
        Pilot* pilot = new Pilot(clientData.line, true);
        __clients[pilot->callsign()] = pilot;
        
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
VatsimDataHandler::findPilot(const QString& _callsign) const {
  if (__clients.contains(_callsign)) {
    return dynamic_cast<Pilot*>(__clients[_callsign]);
  } else {
    return nullptr;
  }
}

const Controller*
VatsimDataHandler::findAtc(const QString& _callsign) const {
  if (__clients.contains(_callsign)) {
    return dynamic_cast<Controller*>(__clients[_callsign]);
  } else {
    return nullptr;
  }
}

Airport*
VatsimDataHandler::findAirport(const QString& _icao) {
  if (__airports.contains(_icao)) {
    return __airports[_icao];
  } else {
    QList<QString> keys = aliases().values(_icao);
    if (_icao.length() < 4)
      keys.prepend(QString("K") % _icao);
    
    for (const QString& k: keys) {
      if (__airports.contains(k))
        return __airports[k];
    }
    
    return nullptr;
  }
}

QList<Airport*>
VatsimDataHandler::airports() const {
  return std::move(__airports.values());
}

Fir*
VatsimDataHandler::findFir(const QString& _icao, bool _fss) {
  QList<QString> keys = aliases().values(_icao);
  if (_icao.length() >= 4)
    keys.prepend(_icao);
  else
    /* Handle USA 3-letter icao contraction */
    keys.prepend(QString("K") % _icao);
  
  for (const QString& k: keys) {
    if (__firs.contains(k)) {
      QList<Fir*> values = __firs.values(k);
      for (Fir* f: values) {
        if (f->isOceanic() == _fss && f->hasValidPosition()) {
          return f;
        }
      }
    }
  }
  
  return nullptr;
}

Uir*
VatsimDataHandler::findUir(const QString& _icao) {
  if (__uirs.contains(_icao))
    return __uirs[_icao];
  
  QList<QString> keys = aliases().values(_icao);
  for (const QString& k: keys) {
    if (__uirs.contains(k))
      return __uirs[k];
  }
  
  return nullptr;
}

QString
VatsimDataHandler::alternameName(const QString& _icao) {
  if (!__alternameNames.contains(_icao))
    return QString();
  else
    return __alternameNames[_icao];
}

QList<Fir*>
VatsimDataHandler::firs() const {
  return std::move(__firs.values());
}

QList<Uir*>
VatsimDataHandler::uirs() const {
  return std::move(__uirs.values());
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

qreal
VatsimDataHandler::fastDistance(
    const qreal& _lat1, const qreal& _lon1,
    const qreal& _lat2, const qreal& _lon2) {
  return qSqrt(
    qPow(_lat1 - _lat2, 2) +
    qPow(_lon1 - _lon2, 2)
  );
}

qreal
VatsimDataHandler::fastDistance(const LonLat& _a, const LonLat& _b) {
  return qSqrt(
    qPow(_a.latitude() - _b.latitude(), 2) +
    qPow(_a.longitude() - _b.longitude(), 2)
  );
}

qreal
VatsimDataHandler::nmDistance(
    const qreal& _lat1, const qreal& _lon1,
    const qreal& _lat2, const qreal& _lon2) {
  
  /* http://www.movable-type.co.uk/scripts/latlong.html */
  static constexpr qreal R = 3440.06479191; // nm
  
  return qAcos(
      qSin(_lat1) * qSin(_lat2) +
      qCos(_lat1) * qCos(_lat2) *
      qCos(_lon2 - _lon1)
    ) * R;
}

qreal
VatsimDataHandler::nmDistance(const LonLat& _a, const LonLat& _b) {
  /* http://www.movable-type.co.uk/scripts/latlong.html */
  static constexpr qreal R = 3440.06479191; // nm
  
  return qAcos(
      qSin(_a.latitude()) * qSin(_b.latitude()) +
      qCos(_a.latitude()) * qCos(_b.latitude()) *
      qCos(_b.longitude() - _a.longitude())
    ) * R;
}

void
VatsimDataHandler::requestDataUpdate() {
  if (__downloader->isWorking())
    __downloader->abort();
  
  emit vatsimDataDownloading();
}

void
VatsimDataHandler::__readAliasFile(const QString& _fName) {
  VatsinatorApplication::log("Reading \"alias\" file...");
  
  QFile file(_fName);
  
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    notifyWarning(tr("File %1 could not be opened! Please reinstall the application.").arg(file.fileName()));
    return;
  }
  
  QJson::Parser parser;
  bool ok;
  
  QVariant content = parser.parse(&file, &ok);
  if (!ok) {
    notifyWarning(tr("File %1 contains errors. Please reinstall the application.").arg(file.fileName()));
    return;
  }
  
  auto list = content.toList();
  for (QVariant& a: list) {
    auto map = a.toMap();
    QString target = map["target"].toString();
    auto aList = map["alias"].toList();
    for (QVariant& b: aList) {
      if (b.canConvert<QString>()) {
        __aliases.insert(target, b.toString());
      } else {
        auto aMap = b.toMap();
        auto aaList = aMap["alias"].toList();
        QString name = aMap["name"].toString();
        for (QVariant& c: aaList) {
          __aliases.insert(target, c.toString());
          __alternameNames.insert(c.toString(), name);
        }
      }
    }
  }
  
  file.close();
  
  VatsinatorApplication::log("Finished reading \"alias\" file.");
}

void
VatsimDataHandler::__readCountryFile(const QString& _fName) {
  VatsinatorApplication::log("Reading \"country\" file...");
  
  QFile file(_fName);
  
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    emit localDataBad(tr("File %1 could not be opened!").arg(_fName));
    return;
  }
  
  while (!file.atEnd()) {
     QString line = QString::fromUtf8(file.readLine()).simplified();
    
    if (line[0] == '#' || line.isEmpty())
      continue;
    
    countries.insert(
              line.section(' ', -1),
              line.section(' ', 0, -2)
              );
  }
  
  file.close();
  
  VatsinatorApplication::log("Finished reading \"country\" file.");
}

void
VatsimDataHandler::__readFirFile(const QString& _fName) {
  VatsinatorApplication::log("Reading \"fir\" file...");
  
  QFile file(_fName);
  
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    emit localDataBad(tr("File %1 could not be opened!").arg(_fName));
    return;
  }
  
  while (!file.atEnd()) {
     QString line = QString::fromUtf8(file.readLine()).simplified();
    
    if (line[0] == '#' || line.isEmpty())
      continue;
    
    QString icao = line.section(' ', 0, 0);
    
    Fir* currentFir = findFir(icao);
    if (currentFir) {
      currentFir->setName(line.section(' ', 1));
      
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
      currentFir->setName(line.section(' ', 1));
      currentFir->setCountry(countries[icao.left(2)]);
      currentFir->fixupName();
    }
  }
  
  file.close();
  
  VatsinatorApplication::log("Finished reading \"fir\" file.");
}

void
VatsimDataHandler::__readUirFile(const QString& _fileName) {
  VatsinatorApplication::log("Reading \"uir\" file...");
  
  QFile file(_fileName);
  
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    emit localDataBad(tr("File %1 could not be opened!").arg(_fileName));
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
  
  VatsinatorApplication::log("Finished reading \"uir\" file.");
}

void
VatsimDataHandler::__initData() {
  for_each(FirDatabase::getSingleton().firs().begin(),
    FirDatabase::getSingleton().firs().end(),
    [this](const FirRecord& fr) {
      Fir* f = new Fir(&fr);
      __firs.insert(f->icao(), f);
    }
  );
  
  for_each(AirportDatabase::getSingleton().airports().begin(),
    AirportDatabase::getSingleton().airports().end(),
    [this](const AirportRecord& ar) {
      Airport* a = new Airport(&ar);
      __airports.insert(a->icao(), a);
    }
  );
}

void
VatsimDataHandler::__clearData() {
  qDeleteAll(__clients);

  __observers = 0;
}

void
VatsimDataHandler::__loadCachedData() {
  VatsinatorApplication::log("VatsimDataHandler: loading data from cache...");
  
  CacheFile file(CacheFileName);
  if (file.exists()) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      VatsinatorApplication::log("VatsimDataHandler: cache file %s is not readable.",
                                 qPrintable(file.fileName()));
      return;
    }
    
    QString data(file.readAll());
    file.close();
    parseDataFile(data);
  }
  
  VatsinatorApplication::log("VatsimDataHandler: cache restored.");
}

void
VatsimDataHandler::__cleanupClients() {
  for (auto c: __clients) {
    if (!c->isOnline()) {
      __clients.remove(c->callsign());
      c->deleteLater();
    }
  }
}

void
VatsimDataHandler::__slotUiCreated() {
  if (SM::get("network.cache_enabled").toBool() == true)
    __loadCachedData();
  
  __downloader->setProgressBar(vApp()->userInterface()->mainWindow()->progressBar());
  __beginDownload();
}

void
VatsimDataHandler::__beginDownload() {
  VatsinatorApplication::log("VatsimDataHandler: starting download.");
  __downloader->fetchData(getDataUrl());
}

void
VatsimDataHandler::__dataFetched(QString _data) {
  if (__statusFileFetched) {
    if (_data.isEmpty()) {
      emit vatsimDataError();
      return;
    }
    
    parseDataFile(_data);
    emit vatsimDataUpdated();
    
    if (SM::get("network.cache_enabled").toBool())
      FileManager::cacheData(CacheFileName, _data);
    
    MetarListModel::getSingleton().updateAll();
  } else {
    parseStatusFile(_data);
  }
}

void
VatsimDataHandler::__handleFetchError() {
  if (__statusFileFetched) {
    emit vatsimDataError();
  } else {
    if (__statusUrl != QString(NetConfig::Vatsim::backupStatusUrl())) {
      /* Try the backup url */
      __statusUrl = NetConfig::Vatsim::backupStatusUrl();
      __beginDownload();
    } else {
      /* We already tried - there is something else causing the error */
      emit vatsimStatusError();
    }
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
