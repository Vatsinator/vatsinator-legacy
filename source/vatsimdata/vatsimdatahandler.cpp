/*
    vatsimdatahandler.cpp
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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

#include <QtGui>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"

#include "network/plaintextdownloader.h"

#include "modules/modulemanager.h"

#include "ui/pages/miscellaneouspage.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/userinterface.h"

#include "storage/cachefile.h"
#include "storage/settingsmanager.h"

#include "vatsimdata/fir.h"
#include "vatsimdata/uir.h"
#include "vatsimdata/airport/activeairport.h"
#include "vatsimdata/airport/emptyairport.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsimdata/models/metarlistmodel.h"

#include "storage/filemanager.h"

#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "vatsimdatahandler.h"
#include "defines.h"

static const QString CacheFileName = "lastdata";

FlightTableModel* VatsimDataHandler::emptyFlightTable = new FlightTableModel();
ControllerTableModel* VatsimDataHandler::emptyControllerTable = new ControllerTableModel();

static QMap<QString, QString> countries; // used by __readCountryFile() and __readFirFile()


VatsimDataHandler::VatsimDataHandler() :
    __flights(new FlightTableModel()),
    __atcs(new ControllerTableModel()),
    __statusUrl(NetConfig::Vatsim::statusUrl()),
    __observers(0),
    __statusFileFetched(false),
    __airports(AirportDatabase::getSingleton()),
    __firs(FirDatabase::getSingleton()),
    __downloader(new PlainTextDownloader()) {
  
  connect(VatsinatorApplication::getSingletonPtr(), SIGNAL(uiCreated()),
          this,                                     SLOT(loadCachedData()));
  connect(VatsinatorApplication::getSingletonPtr(), SIGNAL(uiCreated()),
          this,                                     SLOT(__slotUiCreated()));
  connect(__downloader,                             SIGNAL(finished(QString)),
          this,                                     SLOT(__dataFetched(QString)));
  connect(__downloader,                             SIGNAL(fetchError()),
          this,                                     SLOT(__handleFetchError()));
  
  connect(this, SIGNAL(vatsimDataDownloading()), SLOT(__beginDownload()));
  connect(this, SIGNAL(localDataBad(QString)), SLOT(__reportDataError(QString)));
  
  __timer.setSingleShot(true);
  connect(&__timer,     SIGNAL(timeout()),
          this,         SIGNAL(vatsimDataDownloading()));
}

VatsimDataHandler::~VatsimDataHandler() {
  __clearData();
  
  delete __downloader;
  
  qDeleteAll(__uirs);

  delete __atcs;
  delete __flights;
  
  delete VatsimDataHandler::emptyFlightTable;
  delete VatsimDataHandler::emptyControllerTable;
}

void
VatsimDataHandler::init() {
  auto f = QtConcurrent::run(this, &VatsimDataHandler::__readCountryFile,
                             FileManager::path("data/country"));
  
  QtConcurrent::run(this, &VatsimDataHandler::__readAliasFile,
                    FileManager::path("data/alias"));
  QtConcurrent::run(this, &VatsimDataHandler::__readUirFile,
                    FileManager::path("data/uir"));
  
  f.waitForFinished();
  __readFirFile(FileManager::path("data/fir"));
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
        QString url0 = value;
        __dataServers << url0;
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
  static QRegExp rx("^\\b(UPDATE|RELOAD)\\b\\s?=\\s?\\b(.+)\\b$");
  
  __clearData();

  VatsinatorApplication::log("Data length: %i.", _data.length());

  QStringList tempList = _data.split('\n', QString::SkipEmptyParts);
  
  DataSections section = None;

  for (QString& temp: tempList) {
    if (temp.startsWith(';'))
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
    http://www.pcflyer.net/DataFeed/vatsim-servers.txt
    switch (section) {
      case DataSections::General: {
        if (rx.indexIn(temp) >= 0) {
          QString key = rx.cap(1);
          QString value = rx.cap(2);
          
          if (key == "UPDATE") {
            __dateVatsimDataUpdated = QDateTime::fromString(
              value, "yyyyMMddhhmmss");
          } else if (key == "RELOAD") {
            __updateInterval(value.toInt() * 1000 * 60);
          }
        }
        break;
      } // DataSections::General
    
      case DataSections::Clients: {
        QStringList clientData = temp.split(':');
        
        if (clientData.size() < 40) {
          VatsinatorApplication::log("VatsimDataHandler: line invalid: %s", qPrintable(temp));
          emit vatsimDataCorrupted();
          return;
        }
        
        if (clientData[3] == "ATC") {
          Controller* atc = new Controller(clientData);
          
          if (atc->isOk()) {
            __atcs->addStaff(atc);
          } else {
            __observers += 1;
            delete atc;
          }
        } else if (clientData[3] == "PILOT") {
          Pilot* pilot = new Pilot(clientData);
          if (pilot->position().latitude == 0 && pilot->position().longitude == 0)
            delete pilot; // skip unknown flights
          else
            __flights->addFlight(pilot);
        }
        break;
      } // DataSections::Clients
    
      case DataSections::Prefile: {
        QStringList clientData = temp.split(':');
        
        if (clientData.size() < 40) {
          VatsinatorApplication::log("VatsimDataHandler: line invalid: %s", qPrintable(temp));
          emit vatsimDataCorrupted();
          return;
        }
        
        __flights->addFlight(new Pilot(clientData, true));
        break;
      } // DataSections::Prefile
      
      default: {}
    } // switch (section)
  } // for
}

const QString &
VatsimDataHandler::getDataUrl() const {
  if (__statusFileFetched) {
    qsrand(QTime::currentTime().msec());
    return __dataServers[qrand() % __dataServers.size()];
  } else {
    return __statusUrl;
  }
}

const Pilot *
VatsimDataHandler::findPilot(const QString& _callsign) const {
  return __flights->findFlightByCallsign(_callsign);
}

const Controller *
VatsimDataHandler::findAtc(const QString& _callsign) const {
  return __atcs->findAtcByCallsign(_callsign);
}

Uir *
VatsimDataHandler::findUIR(const QString& _icao) {
for (Uir * u: __uirs)
    if (u->icao() == _icao)
      return u;

  return NULL;
}

ActiveAirport *
VatsimDataHandler::addActiveAirport(const QString& _icao) {
  if (!__activeAirports.contains(_icao))
    __activeAirports.insert(_icao, new ActiveAirport(_icao));

  return __activeAirports[_icao];
}

EmptyAirport *
VatsimDataHandler::addEmptyAirport(const QString& _icao) {
  if (!__emptyAirports.contains(_icao))
    __emptyAirports.insert(_icao, new EmptyAirport(_icao));

  return __emptyAirports[_icao];
}

EmptyAirport *
VatsimDataHandler::addEmptyAirport(const AirportRecord* _ap) {
  QString icao(_ap->icao);
  
  if (!__emptyAirports.contains(icao))
    __emptyAirports.insert(icao, new EmptyAirport(_ap));
  
  return __emptyAirports[icao];
}

Airport *
VatsimDataHandler::findAirport(const QString& _icao) {
  if (__activeAirports.contains(_icao))
    return __activeAirports[_icao];
  
  return addEmptyAirport(_icao);
}

int
VatsimDataHandler::clientCount() const {
  return pilotCount() + atcCount() + obsCount();
}

int
VatsimDataHandler::pilotCount() const {
  return __flights->rowCount();
}

int
VatsimDataHandler::atcCount() const {
  return __atcs->rowCount();
}

int
VatsimDataHandler::obsCount() const {
  return __observers;
}

void
VatsimDataHandler::loadCachedData() {
  /* Honor user settings */
  if (SM::get("network.cache_enabled").toBool() == false)
    return;
  
  VatsinatorApplication::log("VatsimDataHandler: loading data from cache...");
  
  CacheFile file(CacheFileName);
  if (file.exists()) {
    FirDatabase::getSingleton().clearAll();
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      VatsinatorApplication::log("VatsimDataHandler: cache file %s is not readable.",
                                 qPrintable(file.fileName()));
      return;
    }
    
    QString data(file.readAll());
    file.close();
    parseDataFile(data);
    ModuleManager::getSingleton().updateData();
  }
  
  VatsinatorApplication::log("VatsimDataHandler: cache restored.");
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
    emit localDataBad(tr("File %1 could not be opened!").arg(_fName));
    return;
  }
  
  while (!file.atEnd()) {
    QString line = QString::fromUtf8(file.readLine()).simplified();
    
    if (line[0] == '#' || line.isEmpty())
      continue;
    
    QStringList data = line.split(' ');
    QString& icao = data.front();
    
    Q_ASSERT(icao.length() == 4);
    
    for (int i = 1; i < data.length(); ++i) {
      if (data[i][0] == '{') {
        __aliases.insert(data[i].mid(1), icao);
      } else if (data[i].toUpper() == data[i]) {
        __aliases.insert(data[i], icao);
      }
      /*
       * TODO Read also named aliases (those in brackets) - they should be displayed
       *      if used. For example, if you have:
       *        LGGG {LGMD Makedonia}
       *      then if user clicks on "LGGG" default name should be shown, otherwise
       *      "Makedonia".
       */
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
    
    Fir* currentFir = __firs.find(icao);
    if (currentFir) {
      currentFir->setName(line.section(' ', 1));
      
      if (currentFir->icao() == "UMKK") // fix for Kaliningrad center
        currentFir->setCountry("Russia");
      else
        currentFir->setCountry(countries[icao.left(2)]);
      
      /*
       * TODO Make this above more convenient, i.e. allow writing some kind of
       *      "exceptions" in the data file.
       */
      
      currentFir->correctName();
    }
    
    // look for some oceanic fir
    currentFir = __firs.find(icao, true);
    if (currentFir) {
      currentFir->setName(line.section(' ', 1));
      currentFir->setCountry(countries[icao.left(2)]);
      currentFir->correctName();
    }
  }
  
  file.close();
  
  VatsinatorApplication::log("Finished reading \"fir\" file.");
}

void
VatsimDataHandler::__readUirFile(const QString& _fName) {
  VatsinatorApplication::log("Reading \"uir\" file...");
  
  QFile file(_fName);
  
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    emit localDataBad(tr("File %1 could not be opened!").arg(_fName));
    return;
  }
  
  while (!file.atEnd()) {
     QString line = QString::fromUtf8(file.readLine()).simplified();
    
    if (line[0] == '#' || line.isEmpty())
      continue;
    
    QStringList data = line.split(' ');
    Uir* uir = new Uir(data[0]);
    
    for (int i = 1; i < data.length(); ++i) {
      if (data[i].toUpper() == data[i]) {
        Fir* fir = __firs.find(data[i]);
        
        if (fir)
          uir->addFir(fir);
        else
          VatsinatorApplication::log("FIR %s could not be found!", data[i].toStdString().c_str());
      } else {
        uir->name().append(data[i] + " ");
      }
    }
    
    __uirs.push_back(uir);
  }
  
  file.close();
  
  VatsinatorApplication::log("Finished reading \"uir\" file.");
}

void
VatsimDataHandler::__clearData() {
  qDeleteAll(__flights->flights()), __flights->clear();
  qDeleteAll(__atcs->staff()), __atcs->clear();
  qDeleteAll(__activeAirports), __activeAirports.clear();
  qDeleteAll(__emptyAirports), __emptyAirports.clear();

  for (Uir* u: __uirs)
    u->clear();
  
  FirDatabase::getSingleton().clearAll();

  __observers = 0;
}

void
VatsimDataHandler::__updateInterval(int _interval) {
  if (SM::get("network.auto_updater").toBool() == true) {
    __timer.setInterval(_interval);
  }
}

void
VatsimDataHandler::__reportDataError(QString _msg) {
  VatsinatorApplication::log(qPrintable(_msg));
  VatsinatorApplication::alert(_msg, true);
}

void
VatsimDataHandler::__slotUiCreated() {
  if (SM::get("network.auto_updater").toBool() == false) {
    __timer.setInterval(SM::get("network.refresh_rate").toInt() * 1000 * 60);
  } else {
    __timer.setInterval(3 * 1000 * 60);
  }
  
  __downloader->setProgressBar(VatsinatorWindow::getSingleton().progressBar());
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
      emit vatsimDataCorrupted();
      return;
    }
    
    parseDataFile(_data);
    emit vatsimDataUpdated();
    __timer.start();
    
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
    emit vatsimDataCorrupted();
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
