/*
    controller.h
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


#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>
#include <QMap>

#include "vatsimdata/client.h"

class Airport;
class Fir;
class Uir;


/*
 * This class represents the ATC.
 */
class Controller : public Client {
  Q_OBJECT

public:
  /* Types */
  enum Facility {
    Atis      = 0x1,
    Del       = 0x2,
    Gnd       = 0x4,
    Twr       = 0x8,
    App       = 0x10,
    Dep       = 0x20,
    Ctr       = 0x40,
    Fss       = 0x80,
    Obs       = 0x100
  };
  Q_DECLARE_FLAGS(Facilities, Facility);
  
  /* Ctors */
  Controller() = delete;
  
  Controller(const QStringList&);
  
  void update(const QStringList&);
  
  /**
   * The frequency that the ATC currently operates on.
   */
  inline const QString& frequency() const { return __frequency; }
  
  /**
   * The client's rating index. Use Controller::ratings to get the
   * real rating.
   */
  inline int rating() const { return __rating; }
  
  /**
   * The icao that the ATC controls. It can be airport or FIR icao.
   */
  inline const QString& icao() const { return __icao; }
  
  /**
   * The current ATIS message.
   */
  inline const QString& atis() const { return __atis; }
  
  /**
   * The airport that the Controller manages.
   * Returns nullptr if the Controller manages Fir or is the Obs, for
   * example.
   */
  inline const Airport* airport() const { return __airport; }
  
  /**
   * The ATC's facility.
   */
  inline Controller::Facility facility() const { return __facility; }
  
  /**
   * Gets the ATC description, ie "New York approach".
   */
  inline const QString& description() const { return __description; }
  
  /**
   * If isOk() returns false it means that the controller could not be
   * assigned any facility nor airport and therefore needs to be
   * removed.
   */
  inline bool isOk() const { return __isOK; }
  
  
  /* Stores ATC ratings.
   * See http://vateud.org/index.php?option=com_content&view=article&id=28&Itemid=133
   */
  static QMap<int, QString> ratings;
  
private:
  void __cleanupAtis();
  void __recognizeDetails();
  void __makeDescription(const Fir*);
  void __makeDescription(const Uir*);
  void __makeDescription(const Airport*);

  static bool __initRatings();

  QString              __frequency;
  int                  __rating;
  QString              __icao;
  QString              __atis;
  const Airport*       __airport;
  Controller::Facility __facility;
  QString              __description;
  bool                 __isOK;
  
  static bool __ratingsInitialized;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(Controller::Facilities);

#endif // CONTROLLER_H
