/*
    vatsimdatahandler.h
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


#ifndef VATSIMDATAHANDLER_H
#define VATSIMDATAHANDLER_H

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>

#include <qmath.h>

#include "singleton.h"

class ActiveAirport;
class Airport;
class AirportDatabase;
class Controller;
class ControllerTableModel;
class EmptyAirport;
class FirDatabase;
class FlightTableModel;
class Pilot;
class PlainTextDownloader;
class Uir;
class VatsinatorApplication;

struct AirportRecord;

class VatsimDataHandler :
    public QObject,
    public Singleton< VatsimDataHandler > {

  /*
   * This class contains data about all connected clients, needed URLs
   * and airports.
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
   * Parses the data files. Must be called after AirportsDatabase::init()
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
   * @param callsign Callsign of the pilot to be found.
   * @return Const-pointer to the Pilot class instance or NULL.
   */
  const Pilot* findPilot(const QString&) const;
  
  /**
   * Finds Controller by callsign and returns pointer. If not found,
   * returns NULL.
   * @param callsign Callsign of the controller to be found.
   * @return Const-pointer to the Controller class instance or NULL.
   */
  const Controller* findAtc(const QString&) const;

  /**
   * Finds UIR by ICAO.
   */
  Uir* findUIR(const QString&);

  /**
   * Adds an airport to the ActiveAirports map (if it doesn't exist already).
   * @param icao ICAO code of the airport to be added.
   * @return Pointer to the ActiveAirport.
   */
  ActiveAirport* addActiveAirport(const QString&);
  
  /**
   * Adds an inactive airport to the EmptyAirport map.
   */
  EmptyAirport* addEmptyAirport(const QString&);
  EmptyAirport* addEmptyAirport(const AirportRecord*);
  
  /**
   * Finds airport with particular ICAO.
   * @param icao Airport ICAO code.
   * @return Pointer to the Airport instance (can be ActiveAirport
   *         or EmptyAirport pointer).
   */
  Airport* findAirport(const QString&);

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

  inline const QMap< QString, ActiveAirport* > &
  getActiveAirports() const { return __activeAirports; }
  
  inline const QMap< QString, EmptyAirport* > &
  getEmptyAirports() const { return __emptyAirports; }

  inline const QMultiMap< QString, QString > &
  getAliases() const { return __aliases; }

  inline const QDateTime&
  getDateDataUpdated() const { return __dateVatsimDataUpdated; }

  inline bool
  statusFileFetched() const { return __statusFileFetched; }

  inline static qreal
  calcDistance(const qreal& _ax, const qreal& _ay,
               const qreal& _bx, const qreal& _by) {
    return qSqrt(
             qPow(_ax - _bx, 2) +
             qPow(_ay - _by, 2)
           );
  }
  
  /* These pointers are used for empty views.
   * Giving the NULL pointer in setModel() removes headers. */
  static FlightTableModel* emptyFlightTable;
  static ControllerTableModel* emptyControllerTable;
  
public slots:
  
  /**
   * Reads cached data.
   */
  void loadCachedData();
  
private:
  
  /**
   * These functions read data files.
   * @param fileName Location of the data file.
   */
  void __readAliasFile(const QString&);
  void __readCountryFile(const QString&);
  void __readFirFile(const QString&);
  void __readUirFile(const QString&);

  /**
   * Removes all data, frees pointers
   */
  void __clearData();
  
  /**
   * TODO Change this shit below to QFlags.
   */
  void __clearFlags(QMap< QString, bool >&);
  

  /* These are vectors of connected clients */
  FlightTableModel*   __flights;
  ControllerTableModel*   __atcs;

  QVector< Uir* >   __uirs;

  /* This is vector of data servers, obtained from status file */
  QVector< QString >  __servers;

  /* This set contains list of active airports, used later by OpenGLWidget */
  QMap< QString, ActiveAirport* > __activeAirports;
  
  /* Inactive airports (no staff, no flights) */
  QMap< QString, EmptyAirport* > __emptyAirports;

  /* This set contains list of aliases. Filled in by init() method */
  QMultiMap< QString, QString > __aliases;

  /* This is URL that we can obtain METAR from */
  QString   __metarURL;

  /* And status.txt */
  QString   __statusURL;

  QDateTime __dateVatsimDataUpdated;

  int       __observers;

  /* Indicates whether the status.txt file was already read or not */
  bool      __statusFileFetched;

  AirportDatabase& __airports;
  FirDatabase&     __firs;
  
  PlainTextDownloader* __downloader;
  
private slots:
  /**
   * Called if local data is bad.
   */
  void __reportDataError(QString);
  
  void __slotUiCreated();
  void __beginDownload();
  void __dataFetched(const QString&);
  
signals:
  
  /**
   * Called after status.txt is parsed.
   */
  void vatsimStatusUpdated();
  
  /**
   * Called when new data is already parsed.
   */
  void vatsimDataUpdated();
  
  /**
   * Incomplete fetch or something like that.
   */
  void dataCorrupted();
  
  /**
   * If local data is corrupted.
   */
  void localDataBad(QString);

};

typedef QMap< QString, ActiveAirport* > AirportsMap;

#endif // VATSIMDATAHANDLER_H
