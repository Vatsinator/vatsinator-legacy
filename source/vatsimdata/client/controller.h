/*
    controller.h
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


#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>
#include <QMap>

#include "vatsimdata/client.h"

struct AirportRecord;
class Fir;
class Uir;

class Controller : public Client {
  
  /*
   * This class represents the ATC.
   */

public:
  /* Types */
  enum Facility {
    ATIS      = 0x1,
    DEL       = 0x2,
    GND       = 0x4,
    TWR       = 0x8,
    APP       = 0x10,
    DEP       = 0x20,
    CTR       = 0x40,
    FSS       = 0x80,
    OBS       = 0x100
  };
  Q_DECLARE_FLAGS(Facilities, Facility);
  
  /* Ctors */
  Controller() = delete;
  
  Controller(const QStringList&);
  
  /**
   * Reimplemented from Client::clientType().
   */
  Client::Type clientType() const override;
  
  inline const QString &
  frequency() const { return __frequency; }
  
  inline int
  rating() const { return __rating; }
  
  inline const QString &
  icao() const { return __icao; }
  
  inline const QString &
  atis() const { return __atis; }
  
  inline const AirportRecord *
  airport() const { return __airport; }
  
  inline Controller::Facility
  facility() const { return __facility; }
  
  inline const QString &
  description() const { return __description; }
  
  inline bool
  isOk() const { return __isOK; }
  
  
  /* Stores ATC ratings.
   * See http://vateud.org/index.php?option=com_content&view=article&id=28&Itemid=133
   */
  static QMap<int, QString> ratings;
  
protected:
  QString              __frequency;
  int                  __rating;
  QString              __icao;
  
  QString              __atis;
  
  const AirportRecord* __airport;
  
  Controller::Facility __facility;
  
  QString              __description;
  
  bool                 __isOK;

private:
  void __cleanupAtis();
  void __setMyIcaoAndFacility();
  void __produceDescription(const Fir*);
  void __produceDescription(const Uir*);
  void __produceDescription(const AirportRecord*);

  static bool __initRatings();

  static bool __ratingsInitialized;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(Controller::Facilities);

#endif // CONTROLLER_H
