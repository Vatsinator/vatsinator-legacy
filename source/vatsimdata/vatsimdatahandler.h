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

#include "vatsimdata/client.h"
#include "ui/notifiable.h"
#include "singleton.h"

class AbstractNotamProvider;
class Airport;
class Controller;
class ControllerTableModel;
class DecisionEvent;
class Fir;
class FlightTableModel;
class Pilot;
class PlainTextDownloader;
class UpdateScheduler;
class Uir;
class WeatherForecastInterface;
class VatsinatorApplication;

struct AirportRecord;

/**
 * The VatsimDataHandler class is responsible for handling all Vatsim specific
 * data. Not only keeps it track of all connected clients, but it also
 * downloads data automatically when needed, parses local files and provides
 * some math functions.
 */
class VatsimDataHandler : public QObject, public Notifiable {
  Q_OBJECT
  
  /**
   * This property defines whether all data that is needed for the recognition
   * mechanisms is loaded. If value of the property is false, functions like
   * findAirport() or findFir() won't function properly.
   */
  Q_PROPERTY(bool initialized READ isInitialized NOTIFY initialized)
  
  /**
   * Holds the number of connected VATSIM clients (pilotCount + atcCount + obsCount).
   */
  Q_PROPERTY(int clientCount READ clientCount)
  
  /**
   * Holds the number of connected VATSIM pilots.
   */
  Q_PROPERTY(int pilotCount READ pilotCount)
  
  /**
   * Holds the number of connected ATCs.
   */
  Q_PROPERTY(int atcCount READ atcCount)
  
  /**
   * Holds the number of connected observers.
   */
  Q_PROPERTY(int obsCount READ obsCount)
  
  /**
   * Date and time when the currently loaded data was fetched.
   * \todo Rename it to something shorter.
   */
  Q_PROPERTY(QDateTime dateDataUpdated READ dateDataUpdated)
  
signals:
  
  /**
   * Emitted just after all data is read.
   * \sa isInitialized().
   */
  void initialized();
  
  /**
   * Emitted after status.txt is parsed.
   */
  void vatsimStatusUpdated();
  
  /**
   * Emitted if the status file could not be fetched correctly.
   */
  void vatsimStatusError();
  
  /**
   * Emitted when Vatsim data starts to be downloaded.
   */
  void vatsimDataDownloading();
  
  /**
   * Emitted when new data is already loaded.
   */
  void vatsimDataUpdated();
  
  /**
   * Emitted on incomplete fetch or some network error.
   */
  void vatsimDataError();

public:
  /**
   * Default ctor.
   */
  VatsimDataHandler(QObject* parent = nullptr);
  
  /**
   * Destructor deletes all pointers.
   */
  ~VatsimDataHandler();
  
  /**
   * Parses the data files.
   * Called by VatsinatorApplication only, once, at the very beginning.
   */
  void initialize();
  
  /**
   * This function parses the raw "status.txt" file.
   * 
   * \param content The file's contents.
   * \todo Move to private scope.
   */
  void parseStatusFile(const QString& content);
  
  /**
   * This function parses the data file.
   * 
   * \param content The file's contents.
   * \todo Move to private scope.
   */
  void parseDataFile(const QString& content);
  
  /**
   * Chooses randomly one of the URLs.
   * 
   * \return The randomly chosen data file URL.
   * \todo Move to private scope.
   */
  const QString& getDataUrl() const;
  
  /**
   * Creates the new model and populates it with all flights that are online.
   * 
   * \return The newly allocated and filled model; needs to be deleted afterwards.
   * \sa controllerTableModel().
   */
  FlightTableModel* flightTableModel() const;
  
  /**
   * Creates the new model and populates it with all controllers that are online.
   * 
   * \return The newly allocated and filled model; needs to be deleted afterwards.
   * \sa flightTableModel().
   */
  ControllerTableModel* controllerTableModel() const;
  
  /**
   * Looks for pilot by callsign.
   * 
   * \param callsign Callsign of the pilot to be found.
   * \return Pointer to the Pilot class instance or _nullptr_ if no pilot was found.
   * \sa findAtc() and findAirport().
   */
  const Pilot* findPilot(const QString& callsign) const;
  
  /**
   * Looks for the ATC.
   * 
   * \param callsign Callsign of the controller to be found.
   * \return Pointer to the Controller class instance or _nullptr_ if no pilot was found.
   * \sa findPilot().
   */
  const Controller* findAtc(const QString& callsign) const;
  
  /**
   * Finds airport with particular _icao_ code or any airport that the given
   * _icao_ is alias of.
   * 
   * \param icao Airport ICAO code.
   * \return Pointer to the Airport instance or _nullptr_ if nothing was found.
   * \sa findPilot() and findAtc().
   */
  Airport* findAirport(const QString& icao);
  
  /**
   * @return List of all airports recognized by Vatsinator.
   */
  QList<Airport*> airports() const;
  
  /**
   * Finds FIR that matches the given _icao_ code or any FIR that the given
   * _icao_ is alias of.
   * 
   * \param icao The FIR's ICAO code.
   * \param fss If set to true, this function will look only for FSS FIRs. Defaults to false.
   * \return Pointer to the Fir instance or _nullptr_ if nothing was found.
   * \sa findUir() and findAirport().
   */
  Fir* findFir(const QString& icao, bool fss = false);
  
  /**
   * Finds UIR that matches the given _icao_ code or any UIR that the given
   * _icao_ is alias of.
   * 
   * \param icao The UIR's ICAO code.
   * \return Pointer to the Uir instance or _nullptr_ if nothing was found.
   * \sa findFir() and findAirport().
   */
  Uir* findUir(const QString& icao);
  
  /**
   * Finds alternate name for the given ICAO. If nothing was found,
   * empty string is returned.
   * 
   * \param icao The ICAO code.
   * \return The alternate name.
   */
  QString alternameName(const QString& icao);
  
  /**
   * \return List of all FIRs known by Vatsinator.
   */
  QList<Fir*> firs() const;
  
  /**
   * \return List of all UIRs known by Vatsinator.
   */
  QList<Uir*> uirs() const;
  
  /**
   * Running instance of notam provider.
   */
  AbstractNotamProvider* notamProvider();
  
  /**
   * Running instance of weather forecast provider, or _nullptr_ if user did
   * not select any.
   */
  WeatherForecastInterface* weatherForecastProvider();
  
  /**
   * Custom event handler.
   */
  bool event(QEvent*) override;
  
  /**
   * Map (Callsign <-> instance pairs) of connected clients.
   */
  inline const QMap<QString, Client*>& clients() { return __clients; }
  
  /**
   * List of only new clients, i.e. that showed up in the last update.
   */
  inline const QList<Client*>& newClients() { return __newClients; }

  /**
   * Gets base URL for METAR reports to download.
   */
  inline const QString& metarUrl() const { return __metarUrl; }
  
  /**
   * Time between data reloads, in minutes.
   */
  inline int timeToReload() const { return __reload; }
  
  /**
   * Current timestamp is updated every time VatsimDataHandler receives new
   * data file and is guaranteed to be unique. Each client should contain
   * its own copy of this variable and update it on every update() call.
   * This way we can track outdated clients that are not longer logged-in
   * to Vatsim, but still occupy room in Vatsinator memory.
   */
  inline const qint64& currentTimestamp() const {
    return __currentTimestamp;
  }

  /**
   * Returns true if status.txt is already fetched & parsed.
   */
  inline bool statusFileFetched() const { return __statusFileFetched; }
  
  /**
   * Returns false before initialized() signal is emitted.
   */
  inline bool isInitialized() const { return __initialized; }
  
  int clientCount() const;
  int pilotCount() const;
  int atcCount() const;
  int obsCount() const;
  
  inline const QDateTime& dateDataUpdated() const {
    return __dateVatsimDataUpdated;
  }
  
  /**
   * Calculates the distance between two points. The unit is undefined.
   * 
   * \note If you need specific unit, i.e. nautical miles, use
   * VatsimDataHandler::nmDistance() function.
   * 
   * \param lat1 Latitude of the first point.
   * \param lon1 Longitude of the first point.
   * \param lat2 Latitude of the second point.
   * \param lon2 Longitude of the second point.
   * \return Distance between these two points.
   */
  static qreal fastDistance(const qreal& lat1, const qreal& lon1,
                            const qreal& lat2, const qreal& lon2);
  
  /**
   * Calculates the distance between two points. The unit is undefined.
   * 
   * \note If you need specific unit, i.e. nautical miles, use
   * VatsimDataHandler::nmDistance() function.
   * 
   * \param a First point in the global coordinates.
   * \param b Second point in the global coordinates.
   * \return Distance between these two points.
   */
  static qreal fastDistance(const LonLat& a, const LonLat& b);
  
  /**
   * Calculates distance between two points, expressed in
   * nautical miles.
   * 
   * \note If you don't need the distance specifically in nautical miles
   * (i.e. you just need to compare two distances), use VatsimDataHandler::distance()
   * instead, as it is a lot quicker.
   * 
   * \note All coordinates must be in radians.
   * 
   * \param lat1 Latitude of the first point.
   * \param lon1 Longitude of the first point.
   * \param lat2 Latitude of the second point.
   * \param lon2 Longitude of the second point.
   * \return Distance between these two points.
   */
  static qreal nmDistance(const qreal& lat1, const qreal& lon1, const qreal& lat2, const qreal& lon2);
  
  /**
   * Calculates distance between two points, expressed in
   * nautical miles.
   * 
   * \note If you don't need the distance specifically in nautical miles
   * (i.e. you just need to compare two distances), use VatsimDataHandler::distance()
   * instead, as it is a lot quicker.
   * 
   * \note All coordinates must be in radians.
   * 
   * \param a First point.
   * \param b Second point.
   * @return Distance between these two points.
   */
  static qreal nmDistance(const LonLat& a, const LonLat& b);
  
public slots:
  
  /**
   * This is the safest method to refresh the Vatsim data.
   * If data is being already downloaded, it is aborted and
   * the new download is queued.
   */
  void requestDataUpdate();
  
protected:
  virtual void userDecisionEvent(DecisionEvent* event);
  
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
   * The following functions read data files.
   * \param fileName Location of the data file.
   */
  void __readAliasFile(const QString& fileName);
  void __readCountryFile(const QString& fileName);
  void __readFirFile(const QString& fileName);
  void __readUirFile(const QString& fileName);
  
  /**
   * Loads classes that wrap database records.
   */
  void __initializeData();
  
  /**
   * Goes through all the clients and checks whether they are still online
   * or not. The check is based on the client's timestamp.
   */
  void __cleanupClients();
  
private slots:
  /**
   * Loads data file stored in the cache.
   */
  void __loadCachedData();
  
  /**
   * Just after the UI is created, there are some things we should
   * do right away. The data should be restored from cache, but
   * only if databases are already initialized and the first download
   * can be also started here.
   */
  void __slotUiCreated();
  
  /**
   * Starts the real data download process.
   */
  void __beginDownload();
  
  /**
   * The data file has been fetched.
   */
  void __dataFetched(QString data);
  
  /**
   * If any file can't be fetched.
   */
  void __handleFetchError();
  
  /**
   * Reload forecast provider when user chooses another one.
   */
  void __reloadWeatherForecast();
  
private:  
  
  /*
   * All connected clients
   * Callsign <-> instance pairs
   */
  QMap<QString, Client*> __clients;
  
  /*
   * List of only new clients, i.e. that showed up in the last update.
   */
  QList<Client*> __newClients;
  
  /**
   * List of clients marked as invalid.
   */
  QList<Client*> __invalidClients;
  
  /*
   * All airports, each instance wraps the record in the database.
   * ICAO <-> instance pairs
   */
  QMap<QString, Airport*> __airports;
  
  /*
   * All FIRs, each instance wraps the record in the database.
   * ICAO <-> instance pairs
   */
  QMultiMap<QString, Fir*> __firs;
  
  /*
   * UIRs are stored in a separate file.
   * ICAO <-> instance pairs
   */
  QMap<QString, Uir*> __uirs;
  
  /* This is vector of data servers, obtained from the status file */
  QVector<QString> __dataServers;
  
  /* Map of ICAO aliases */
  QMultiMap<QString, QString> __aliases;
  
  /* Map of alternate names for given ICAO */
  QMap<QString, QString> __alternameNames;
  
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
  
  /* False before initialized() method is called */
  bool __initialized;
  
  PlainTextDownloader* __downloader;
  UpdateScheduler*     __scheduler;

  AbstractNotamProvider*        __notamProvider;
  WeatherForecastInterface*     __weatherForecast;
};

#endif // VATSIMDATAHANDLER_H
