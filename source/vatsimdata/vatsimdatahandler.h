/*
    vatsimdatahandler.h
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


#ifndef VATSIMDATAHANDLER_H
#define VATSIMDATAHANDLER_H

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QMultiMap>

#include <qmath.h>

#include "vatsimdata/client.h"
#include "singleton.h"

class AbstractNotamProvider;
class Airport;
class Controller;
class ControllerTableModel;
class Fir;
class FlightTableModel;
class Pilot;
class PlainTextDownloader;
class UpdateScheduler;
class VatsinatorApplication;

struct AirportRecord;

class VatsimDataHandler :
    public QObject,
    public Singleton<VatsimDataHandler> {

  /*
   * This class contains data about all connected clients, needed URLs
   * and airports.
   */

  Q_OBJECT
  
signals:
  
  /**
   * All data is read.
   */
  void initialized();
  
  /**
   * Called when vatsim data starts to be downloaded.
   */
  void vatsimDataDownloading();
  
  /**
   * Called after status.txt is parsed.
   */
  void vatsimStatusUpdated();
  
  /**
   * Status file could not be fetched correctly.
   */
  void vatsimStatusError();
  
  /**
   * Called when new data is already parsed.
   */
  void vatsimDataUpdated();
  
  /**
   * Incomplete fetch or something like that.
   */
  void vatsimDataError();
  
  /**
   * If local data is corrupted.
   */
  void localDataBad(QString);

public:
  /**
   * Default ctor.
   */
  VatsimDataHandler(QObject* = nullptr);
  
  /**
   * Destructor deletes all pointers.
   */
  ~VatsimDataHandler();
  
  /**
   * Parses the data files.
   */
  void init();
  
  /**
   * This function parses the raw "status.txt" file.
   */
  void parseStatusFile(const QString&);
  
  /**
   * This function parses the data file.
   */
  void parseDataFile(const QString&);
  
  /**
   * Chooses randomly one of URLs and returns it.
   */
  const QString& getDataUrl() const;
  
  /**
   * Creates the new model and populates it with all flights online.
   */
  FlightTableModel* flightTableModel() const;
  
  /**
   * Creates the new model and populates it with all controllers online.
   */
  ControllerTableModel* controllerTableModel() const;
  
  /**
   * Finds pilot by callsign and returns pointer. If not found, returns
   * nullptr.
   * @param callsign Callsign of the pilot to be found.
   * @return Const-pointer to the Pilot class instance or nullptr.
   */
  const Pilot* findPilot(const QString&) const;
  
  /**
   * Finds Controller by callsign and returns pointer. If not found,
   * returns nullptr.
   * @param callsign Callsign of the controller to be found.
   * @return Const-pointer to the Controller class instance or nullptr.
   */
  const Controller* findAtc(const QString&) const;
  
  /**
   * Finds airport with particular _icao_ code or any airport that the given
   * _icao_ is alias of.
   * @param icao Airport ICAO code.
   * @return Pointer to the Airport instance or nullptr if could not find.
   */
  Airport* findAirport(const QString&);
  
  /**
   * @return List of all airports recognized by Vatsinator.
   */
  QList<Airport*> airports() const;
  
  /**
   * Finds FIR that matches the given _icao_ code or any FIR that the given
   * _icao_ is alias of.
   * @param icao The FIR ICAO code.
   * @return Pointer to the Fir instance or nullptr if no matches.
   */
  Fir* findFir(const QString&, bool = false);
  
  /**
   * @return List of all FIRs known by Vatsinator.
   */
  QList<Fir*> firs() const;

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
   * Running instance of notam provider.
   */
  AbstractNotamProvider* notamProvider();
  
  /**
   * Calculates the distance between two points. The unit is undefined.
   * 
   * NOTE: If you need specific unit, i.e. nautical miles, use
   * VatsimDataHandler::nmDistance() function.
   * 
   * @param lat1 Latitude of the first point.
   * @param lon1 Longitude of the first point.
   * @param lat2 Latitude of the second point.
   * @param lon2 Longitude of the second point.
   * @return Distance between these two points.
   */
  static qreal fastDistance(const qreal&, const qreal&, const qreal&, const qreal&);
  
  /**
   * Calculates the distance between two points. The unit is undefined.
   * 
   * NOTE: If you need specific unit, i.e. nautical miles, use
   * VatsimDataHandler::nmDistance() function.
   * 
   * @param a First point in the global coordinates.
   * @param b Second point in the global coordinates.
   * @return Distance between these two points.
   */
  static qreal fastDistance(const LonLat&, const LonLat&);
  
  /**
   * Calculates distance between two points, expressed in
   * nautical miles.
   * 
   * NOTE: If you don't need the distance specifically in nautical miles
   * (i.e. you just need to compare two distances), use VatsimDataHandler::distance()
   * instead, as it is a lot quicker.
   * 
   * NOTE: All coordinates must be in radians.
   * 
   * @param lat1 Latitude of the first point.
   * @param lon1 Longitude of the first point.
   * @param lat2 Latitude of the second point.
   * @param lon2 Longitude of the second point.
   * @return Distance between these two points.
   */
  static qreal nmDistance(const qreal&, const qreal&, const qreal&, const qreal&);
  
  /**
   * Map (Callsign <-> instance pairs) of connected clients.
   */
  inline const QMap<QString, Client*>& clients() { return __clients; }
  
  /**
   * List of only new clients, i.e. that showed up in the last update.
   */
  inline const QList<Client*>& newClients() { return __newClients; }

  /**
   * Returns an URL to where METARs can be fetched from.
   */
  inline const QString& metarUrl() const { return __metarUrl; }

  /**
   * Gives access to all aliases, stored in "data/alias" file.
   */
  inline const QMultiMap<QString, QString>& aliases() const {
    return __aliases;
  }
  
  /**
   * Time between data reloads, in minutes.
   */
  inline int reload() const { return __reload; }
  
  /**
   * Returns last Vatsim data update date and time.
   */
  inline const QDateTime& dateDataUpdated() const {
    return __dateVatsimDataUpdated;
  }
  
  /**
   * Current timestamp is updated every time VatsimDataHandler receives new
   * data file and is guaranteed to be unique. Each client should contain
   * its own copy of this variable and update it on every update() call.
   * This way we can track outdated clients that are not longer logged-in
   * to Vatsim, but still have place in Vatsinator memory.
   */
  inline const qint64& currentTimestamp() const {
      return __currentTimestamp;
  }

  /**
   * Returns true if status.txt is already fetched & parsed.
   */
  inline bool statusFileFetched() const { return __statusFileFetched; }
  
  /* These pointers are used for empty views.
   * Giving the NULL pointer in setModel() removes headers. */
  static FlightTableModel* emptyFlightTable;
  static ControllerTableModel* emptyControllerTable;
  
public slots:
  
  /**
   * This is the safest method to refresh the Vatsim data.
   * If data is being already downloaded, it is aborted and
   * the new download is queued.
   */
  void requestDataUpdate();
  
private:
  
  /**
   * Sections of the data file. Each section is defined as "!SECTION:"
   * For example:
   *   !CLIENTS:
   */
  enum DataSections {
    None,
    General,
    Clients,
    Prefile
  };
  
  /**
   * Struct that keeps some raw info about client that we need during data file
   * parsing process.
   */
  struct RawClientData {
    RawClientData(const QString&);
    
    QStringList line;
    bool valid;
    QString callsign;
    enum { Pilot, Atc } type;
  };
  
  /**
   * These functions read data files.
   * @param fileName Location of the data file.
   */
  void __readAliasFile(const QString&);
  void __readCountryFile(const QString&);
  void __readFirFile(const QString&);
  
  
  
  /**
   * Loads classes that wrap database records.
   */
  void __initData();

  /**
   * Removes all data, frees pointers.
   */
  void __clearData();
  
  /**
   * Loads data file stored in the cache.
   */
  void __loadCachedData();
  
  /**
   * Goes through all the clients and checks whether they are still online
   * or not. The check is based on the client's timestamp.
   */
  void __cleanupClients();
  
private slots:
  void __slotUiCreated();
  void __beginDownload();
  void __dataFetched(QString);
  
  /**
   * If any file can't be fetched.
   */
  void __handleFetchError();
  
private:  
  
  /*
   * All connected clients
   * Callsign <-> instance pairs
   */
  QMap<QString, Client*>        __clients;
  
  /*
   * List of only new clients, i.e. that showed up in the last update.
   */
  QList<Client*>                __newClients;
  
  /*
   * All airports, each instance wraps the record in the database.
   * ICAO <-> instance pairs
   */
  QMap<QString, Airport*>       __airports;
  
  /**
   * All FIRs, each instance wraps the record in the database.
   * ICAO <-> instance pairs
   */
  QMultiMap<QString, Fir*>      __firs;
  
  /* This is vector of data servers, obtained from the status file */
  QVector<QString>  __dataServers;
  
  /* This set contains list of aliases. Filled in by init() method */
  QMultiMap<QString, QString> __aliases;
  
  /* This is URL that we can obtain METAR from */
  QString   __metarUrl;
  
  /* And status.txt */
  QString   __statusUrl;
  
  /* Minutes to next reload, as stated in data file */
  int __reload;
  
  /* Last time Vatsim data was refreshed.
   * Got from data file.
   */
  QDateTime __dateVatsimDataUpdated;
  
  /* The timestamp is used to describe every data package, so clients can sync to this. */
  qint64 __currentTimestamp;
  
  /* Observer count */
  int __observers;
  
  /* Client cound */
  int __clientCount;
  
  /* Indicates whether the status.txt file was already read or not */
  bool __statusFileFetched;
  
  PlainTextDownloader* __downloader;
  UpdateScheduler*     __scheduler;
  
  AbstractNotamProvider* __notamProvider;
  
};

#endif // VATSIMDATAHANDLER_H
