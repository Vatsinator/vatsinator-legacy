/*
 * vatsimdatadocument.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#include <QtCore>

#include "vatsimdatadocument.h"

namespace {
  /**
   * Sections of the data file. Each section is defined as "!SECTION:"
   * For example:
   *   !CLIENTS:
   */
  enum DataSections { None, General, Clients, Prefile, Servers, VoiceServers };
}

VatsimDataDocument::VatsimDataDocument(QByteArray data, QObject* parent) :
    QObject(parent),
    __data(qMove(data)),
    __isValid(false) {
  __parse();
}

void
VatsimDataDocument::__parse() {
  static QRegExp rx("^\\b(UPDATE|RELOAD|CONNECTED CLIENTS)\\b\\s?=\\s?\\b(.+)\\b$");
  
  if (__data.isEmpty()) {
    qWarning("VatsimDataDocument: trying to parse an empty document");
    return;
  }
  
  QStringList tempList = QString(__data).split(QRegExp("\r?\n"), QString::SkipEmptyParts);
  DataSections section = None;
  
  for (auto& temp: tempList) {
    if (temp.startsWith(';')) // comment
      continue;
    
    if (temp.startsWith('!')) { // section
      section = None;
      
      if (temp.contains("GENERAL"))
        section = General;
      else if (temp.contains("CLIENTS"))
        section = Clients;
      else if (temp.contains("PREFILE"))
        section = Prefile;
      else if (temp.contains("SERVERS"))
        section = Servers;
      else if (temp.contains("VOICE SERVERS"))
        section = VoiceServers;
      else
        qWarning("VatsimDataDocument: unknown section: %s", qPrintable(temp));
      
      continue;
    }
    
    switch (section) {
      case General:
        if (rx.indexIn(temp) >= 0) {
          QString key = rx.cap(1);
          QString value = rx.cap(2);
          
          if (key == "UPDATE") {
            __update = QDateTime::fromString(value, "yyyyMMddhhmmss");
          } else if (key == "RELOAD") {
            bool ok;
            __reload = value.toInt(&ok);
            Q_ASSERT(ok);
            Q_UNUSED(ok);
          } else if (key == "CONNECTED CLIENTS") {
            bool ok;
            __connectedClients = value.toInt(&ok);
            Q_ASSERT(ok);
            Q_UNUSED(ok);
          }
        }
        break;
        
      case Clients: {
        ClientLine line(temp);
        
        if (!line.valid) {
          qWarning("VatsimDataDocument: error parsing line: %s", qPrintable(temp));
          continue;
        }
        
        __clients << qMove(line);
        break;
      }
      
      case Prefile: {
        ClientLine line(temp);
        
        if (!line.valid) {
          qWarning("VatsimDataDocument: error parsing line: %s", qPrintable(temp));
          continue;
        }
        
        __prefile << qMove(line);
        break;
      }
      
      case Servers:
      case VoiceServers:
        break;
      
      default:
        Q_UNREACHABLE();
    }
  }
  
  if (__connectedClients == __clients.size())
    __isValid = true;
}

VatsimDataDocument::ClientLine::ClientLine(const QString& data) {
  line = data.split(':');
  valid = line.size() == 42;
  if (valid) {
    callsign = line[0];
    type = line[3] == "ATC" ? Atc : Pilot;
  }
}
