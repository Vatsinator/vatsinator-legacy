/*
 * netconfig.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NETCONFIG_H
#define NETCONFIG_H

class NetConfig {
public:
  
  class Vatsinator {
  public:
    
    static constexpr const char* homePageUrl() {
      return "http://vatsinator.eu.org/";
    }
  
    static constexpr const char* repoUrl() {
      return "http://repo.vatsinator.eu.org/";
    }
    
    static constexpr const char* statsUrl() {
      return "http://stats.vatsinator.eu.org/";
    }
    
    static constexpr const char* apiUrl() {
      return "http://api.vatsinator.eu.org/";
    }
  
  };
  
  class Vatsim {
  public:
    
    static constexpr const char* statusUrl() {
      return "http://status.vatsim.net/status.txt";
    }
    
    /**
     * URL to copy of status.txt in case vatsim servers are out of order
     */
    static constexpr const char* backupStatusUrl() {
      return "http://repo.vatsinator.eu.org/status.txt";
    }
    
  };
  
  class Vataware {
  public:
    
    /**
     * @param %1 Airport icao code.
     */
    static constexpr const char* airportUrl() {
      return "http://www.vataware.com/airport.cfm?airport=%1";
    }
    
    /**
     * @param %1 The pilot's Vatsim PID.
     */
    static constexpr const char* pilotUrl() {
      return "http://www.vataware.com/pilot.cfm?cid=%1";
    }
    
  };
  
};

#endif // NETCONFIG_H
