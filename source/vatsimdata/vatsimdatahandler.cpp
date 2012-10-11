/*
    vatsimdatahandler.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include "vatsimdata/fir.h"
#include "vatsimdata/uir.h"

#include "vatsimdata/airport/activeairport.h"
#include "vatsimdata/airport/emptyairport.h"

#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"

#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "vatsinatorapplication.h"

#include "vatsimdatahandler.h"
#include "defines.h"

QMap< QString, QString > VatsimDataHandler::__dataFiles;
bool VatsimDataHandler::__fileNamesInitialized = VatsimDataHandler::__initFileNames();

static QMap< QString, QString > countries; // used by __readCountryFile() and __readFirFile()


VatsimDataHandler::VatsimDataHandler() :
    __flights(new FlightTableModel()),
    __atcs(new ControllerTableModel()),
    __statusURL(VATSIM_STATUS_URL),
    __observers(0),
    __statusFileFetched(false),
    __airports(AirportDatabase::getSingleton()),
    __firs(FirDatabase::getSingleton()) {}

VatsimDataHandler::~VatsimDataHandler() {
  __clearData();

  while (!__uirs.empty())
    delete __uirs.back(), __uirs.pop_back();

  delete __atcs;
  delete __flights;
}

void
VatsimDataHandler::init() {
  __readAliasFile(__dataFiles["alias"]);
  __readCountryFile(__dataFiles["country"]);
  __readFirFile(__dataFiles["fir"]);
  __readUirFile(__dataFiles["uir"]);
}

void
VatsimDataHandler::parseStatusFile(const QString& _statusFile) {
  QStringList tempList = _statusFile.split('\n', QString::SkipEmptyParts);

for (QString & temp: tempList) {
    if (temp.startsWith(';'))
      continue;

    if (temp.startsWith("metar0=")) {
      __metarURL = temp.mid(7);
      __metarURL = __metarURL.simplified();
      continue;
    }

    if (temp.startsWith("url0=")) {
      QString url0 = temp.mid(5);
      url0 = url0.simplified();
      __servers.push_back(url0);

      continue;
    }
  }

  __statusFileFetched = true;
}

void
VatsimDataHandler::parseDataFile(const QString& _data) {
  __clearData();

  VatsinatorApplication::log("Data length: %i.", _data.length());

  QStringList tempList = _data.split('\n', QString::SkipEmptyParts);

  QMap< QString, bool > flags;
  flags["GENERAL"] = false;
  flags["CLIENTS"] = false;
  flags["PREFILE"] = false;

  for (QString & temp: tempList) {
    if (temp.startsWith(';'))
      continue;

    if (temp.startsWith('!')) {
      __clearFlags(flags);

      if (temp.simplified() == "!GENERAL:")
        flags["GENERAL"] = true;
      else if (temp.simplified() == "!CLIENTS:")
        flags["CLIENTS"] = true;
      else if (temp.simplified() == "!PREFILE:")
        flags["PREFILE"] = true;

      continue;
    }

    if (flags["GENERAL"]) {
      if (temp.startsWith("UPDATE")) {
        __dateDataUpdated = QDateTime::fromString(
                              temp.split(' ').back().simplified(),
                              "yyyyMMddhhmmss"
                            );
      }
    } else if (flags["CLIENTS"]) {
      QStringList clientData = temp.split(':');

      if (clientData.size() < 40) {
        emit dataCorrupted();
        return;
      }

      if (clientData[3] == "ATC") {
        Controller* atc = new Controller(clientData);

        if (atc->isOK) {
          __atcs->addStaff(atc);
        } else {
          __observers += 1;
          delete atc;
        }
      } else if (clientData[3] == "PILOT") {
        Pilot* pilot = new Pilot(clientData);
        if (pilot->position.latitude == 0 && pilot->position.longitude == 0)
          delete pilot; // skip unknown flights
        else
          __flights->addFlight(pilot);
      }
    } else if (flags["PREFILE"]) {
      QStringList clientData = temp.split(':');

      if (clientData.size() < 40) {
        emit dataCorrupted();
        return;
      }

      Pilot* pilot = new Pilot(clientData, true);
      __flights->addFlight(pilot);
    }
  }
}

const QString&
VatsimDataHandler::getDataUrl() const {
  if (__statusFileFetched) {
    qsrand(QTime::currentTime().msec());
    return __servers[qrand() % __servers.size()];
  } else {
    return __statusURL;
  }
}

const Pilot *
VatsimDataHandler::findPilot(const QString& _callsign) const {
  return __flights->findFlightByCallsign(_callsign);
}

const Controller *
VatsimDataHandler::findATC(const QString& _callsign) const {
  return __atcs->findATCByCallsign(_callsign);
}

Uir*
VatsimDataHandler::findUIR(const QString& _icao) {
for (Uir * u: __uirs)
    if (u->icao == _icao)
      return u;

  return NULL;
}

ActiveAirport*
VatsimDataHandler::addActiveAirport(const QString& _icao) {
  if (!__activeAirports.contains(_icao))
    __activeAirports.insert(_icao, new ActiveAirport(_icao));

  return __activeAirports[_icao];
}

EmptyAirport*
VatsimDataHandler::addEmptyAirport(const QString& _icao) {
  if (!__emptyAirports.contains(_icao))
    __emptyAirports.insert(_icao, new EmptyAirport(_icao));

  return __emptyAirports[_icao];
}

EmptyAirport*
VatsimDataHandler::addEmptyAirport(const AirportRecord* _ap) {
  QString icao(_ap->icao);
  
  if (!__emptyAirports.contains(icao))
    __emptyAirports.insert(icao, new EmptyAirport(_ap));
  
  return __emptyAirports[icao];
}

Airport*
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
VatsimDataHandler::__readAliasFile(const QString& _fName) {
  QFile file(_fName);
  
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    VatsinatorApplication::alert(
        tr("File") % " " % _fName % " " % tr("could not be opened!"),
        true
      );
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
}

void
VatsimDataHandler::__readCountryFile(const QString& _fName) {
  QFile file(_fName);
  
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    VatsinatorApplication::alert(
        tr("File") % " " % _fName % " " % tr("could not be opened!"),
        true
      );
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
}

void
VatsimDataHandler::__readFirFile(const QString& _fName) {
  QFile file(_fName);
  
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    VatsinatorApplication::alert(
        tr("File") % " " % _fName % " " % tr("could not be opened!"),
        true
      );
    return;
  }
  
  while (!file.atEnd()) {
     QString line = QString::fromUtf8(file.readLine()).simplified();
    
    if (line[0] == '#' || line.isEmpty())
      continue;
    
    QString icao = line.section(' ', 0, 0);
    
    Fir* currentFir = __firs.findFirByIcao(icao);
    if (currentFir) {
      currentFir->setName(line.section(' ', 1));
      
      if (currentFir->getIcao() == "UMKK") // fix for Kaliningrad center
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
    currentFir = __firs.findFirByIcao(icao, true);
    if (currentFir) {
      currentFir->setName(line.section(' ', 1));
      currentFir->setCountry(countries[icao.left(2)]);
      currentFir->correctName();
    }
  }
  
  file.close();
}

void
VatsimDataHandler::__readUirFile(const QString& _fName) {
  QFile file(_fName);
  
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    VatsinatorApplication::alert(
        tr("File") % " " % _fName % " " % tr("could not be opened!"),
        true
      );
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
        Fir* fir = __firs.findFirByIcao(data[i]);
        
        if (fir)
          uir->addFir(fir);
        else
          VatsinatorApplication::log("FIR %s could not be found!", data[i].toStdString().c_str());
      } else {
        uir->name.append(data[i] + " ");
      }
    }
    
    __uirs.push_back(uir);
  }
  
  file.close();
}


void
VatsimDataHandler::__clearData() {
  for (const Pilot * p: __flights->getFlights())
    delete p;

  __flights->clear();

  for (const Controller * c: __atcs->getStaff())
    delete c;

  __atcs->clear();

  for (Uir * u: __uirs)
    u->clear();

  for (auto it = __activeAirports.begin(); it != __activeAirports.end(); ++it)
    delete it.value();

  __activeAirports.clear();
  
  for (auto it = __emptyAirports.begin(); it != __emptyAirports.end(); ++it)
    delete it.value();
  
  __emptyAirports.clear();

  __observers = 0;
}

void
VatsimDataHandler::__clearFlags(QMap< QString, bool >& _flags) {
  for (auto it = _flags.begin(); it != _flags.end(); ++it)
    it.value() = false;
}

bool
VatsimDataHandler::__initFileNames() {
#ifndef Q_OS_DARWIN
  VatsimDataHandler::__dataFiles.insert("alias",
                                        VATSINATOR_PREFIX "data/alias");
  VatsimDataHandler::__dataFiles.insert("country",
                                        VATSINATOR_PREFIX "data/country");
  VatsimDataHandler::__dataFiles.insert("fir",
                                        VATSINATOR_PREFIX "data/fir");
  VatsimDataHandler::__dataFiles.insert("uir",
                                        VATSINATOR_PREFIX "data/uir");
#else
  VatsimDataHandler::__dataFiles.insert("alias",
                   QCoreApplication::applicationDirPath() + "/../Resources/data/alias");
  VatsimDataHandler::__dataFiles.insert("contry",
                   QCoreApplication::applicationDirPath() + "/../Resources/data/country");
  VatsimDataHandler::__dataFiles.insert("fir",
                   QCoreApplication::applicationDirPath() + "/../Resources/data/fir");
  VatsimDataHandler::__dataFiles.insert("uir",
                   QCoreApplication::applicationDirPath() + "/../Resources/data/uir");
#endif // Q_OS_DARWIN
  
  return true;
}
