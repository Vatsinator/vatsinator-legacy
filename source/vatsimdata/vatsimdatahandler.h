/*
    vatsimdatahandler.h
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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


#ifndef VATSIMDATAHANDLER_H
#define VATSIMDATAHANDLER_H

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"

#include "vatsimdata/airport.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/uir.h"

#include "vatsimdata/metar.h"

#include "singleton.h"

class VatsinatorApplication;

class VatsimDataHandler :
    public QObject,
    public Singleton< VatsimDataHandler > {

  /*
   * This class contains data about all connected clients, needed URLs and
   * active airports.
   */

  Q_OBJECT

public:
  /**
   * Default ctor.
   */
  VatsimDataHandler();

  /**
   * Destructor deletes all pointers.
   */
  ~VatsimDataHandler();

  /**
   * Parses the vatsinator.dat file. Must be called after AirportsDatabase::init()
   * and FirsDatabase::init().
   */
  void init();

  /**
   * This function parses the raw "status.txt" file. That file is fetched
   * by VatsinatorApplication class and given exactly here.
   */
  void parseStatusFile(const QString&);

  /**
   * This function parses data file. Its URL can be obtained by getDataUrl()
   * function and is choosen randomly from servers list.
   * During parsing new Pilots, Controllers and Airports instances are
   * created.
   */
  void parseDataFile(const QString&);

  /**
   * Chooses randomly one of URLs and returns it.
   */
  const QString& getDataUrl() const;

  /**
   * Finds pilot by callsign and returns pointer. If not found, returns
   * NULL.
   */
  const Pilot* findPilot(const QString&) const;

  /**
   * Finds UIR by ICAO.
   */
  Uir* findUIR(const QString&);

  /**
   * Adds an airport to the ActiveAirports map.
   */
  Airport* addActiveAirport(const QString&);

  /**
   * @return Count of logged-in clients (pilots + controllers + observers).
   */
  int clientCount() const;

  /**
   * @return Count of logged-in pilots.
   */
  int pilotCount() const;

  /**
   * @return Count of logged-in controllers.
   */
  int atcCount() const;

  /**
   * @return Count of logged-in observers.
   */
  int obsCount() const;

  /**
   * Returns an URL to where METARs can be fetched from.
   */
  inline const QString&
  getMetarUrl() const { return __metarURL; }

  /**
   * The following functions return const references to vectors of clients.
   */
  inline FlightTableModel*
  getFlightsModel() { return __flights; }

  inline ControllerTableModel*
  getATCsModel() { return __atcs; }

  inline const QVector< Uir* > &
  getUIRs() const { return __uirs; }

  inline const QMap< QString, Airport* > &
  getActiveAirports() const { return __activeAirports; }

  inline const QMultiMap< QString, QString > &
  getAliases() const { return __aliases; }

  inline const QDateTime&
  getDateDataUpdated() const { return __dateDataUpdated; }

  inline bool
  statusFileFetched() const { return __statusFileFetched; }

  inline static double
  calcDistance(const double& _ax, const double& _ay,
               const double& _bx, const double& _by) {
    return sqrt(
             pow(_ax - _bx, 2) +
             pow(_ay - _by, 2)
           );
  }

signals:
  void dataCorrupted();

private:

  /**
   * Clears the flags used during parsing the data file.
   */
  void __clearFlags(QMap< QString, bool >&);

  /**
   * Removes all data, frees pointers
   */
  void __clearData();

  /* These are vectors of connected clients */
  FlightTableModel*   __flights;
  ControllerTableModel*   __atcs;

  QVector< Uir* >   __uirs;

  /* This is vector of data servers, obtained from status file */
  QVector< QString >  __servers;

  /* This set contains list of active airports, used later by OpenGLWidget */
  QMap< QString, Airport* > __activeAirports;

  /* This set contains list of aliases. Filled in by init() method */
  QMultiMap< QString, QString > __aliases;

  /* This is URL that we can obtain METAR from */
  QString   __metarURL;

  /* And status.txt */
  QString   __statusURL;

  QDateTime __dateDataUpdated;

  int   __observers;

  /* Indicates whether the status.txt file was already read or not */
  bool    __statusFileFetched;

  AirportDatabase&  __airports;
  FirDatabase&    __firs;

  VatsinatorApplication&  __mother;

};

typedef QMap< QString, Airport* > AirportsMap;

#endif // VATSIMDATAHANDLER_H
