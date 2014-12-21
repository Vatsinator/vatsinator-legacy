/*
 * vatsimstatusdocument.cpp
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

#include "vatsimstatusdocument.h"

VatsimStatusDocument::VatsimStatusDocument(QByteArray data, QObject* parent) :
    QObject(parent),
    __data(qMove(data)),
    __valid(false) {
  __parse();
}

void
VatsimStatusDocument::__parse() {
  static QRegExp rx("(msg0|url0|url1|moveto0|metar0)=(.+)\\b");
  
  if (__data.isEmpty())
    return;
  
  QStringList tempList = QString(__data).split('\n', QString::SkipEmptyParts);
  
  for (auto& temp: tempList) {
    if (temp.startsWith(';'))
      continue;
    
    if (rx.indexIn(temp) >= 0) {
      QString key = rx.cap(1);
      QString value = rx.cap(2);
      
      if (key == "moveto0") {
        __moveTo = value;
        return;
      } else if (key == "metar0") {
        __metar = value;
      } else if (key == "url0") {
        __dataFileUrls << qMove(QUrl(value));
        __valid = true;
      } else if (key == "url1") {
        __serverListUrls << qMove(QUrl(value));
      } else if (key == "msg0") {
        __message = value;
      }
    }
  }
}
