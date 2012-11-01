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
  Controller(const QStringList&);
  
  inline Client::Type
  clientType() const { return Client::ATC; }
  
  
  
  enum Facility {
    ATIS      = 1,
    DEL       = 2,
    GND       = 4,
    TWR       = 8,
    APP       = 16,
    DEP       = 32,
    CTR       = 64,
    FSS       = 128,
    OBS       = 256
  };
  Q_DECLARE_FLAGS(Facilities, Facility);
  
  
  
  QString              frequency;
  int                  rating;
  QString              icao;
  
  QString              atis;
  
  const AirportRecord* airport;
  
  Controller::Facility facility;
  
  QString              description;
  
  bool                 isOK;

  /* Stores ATC ratings.
   * See http://vateud.org/index.php?option=com_content&view=article&id=28&Itemid=133
   */
  static QMap< int, QString> ratings;

private:
  void __setMyIcaoAndFacility();
  void __produceDescription(const Fir*);
  void __produceDescription(const Uir*);
  void __produceDescription(const AirportRecord*);

  static bool __initRatings();

  static bool __ratingsInitialized;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(Controller::Facilities);

#endif // CONTROLLER_H
