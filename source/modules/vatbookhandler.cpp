/*
    vatbookhandler.cpp
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

#include <QtGui>

#include "modules/models/bookedatctablemodel.h"
#include "modules/vatbook/bookedcontroller.h"

#include "network/httphandler.h"

#include "vatbookhandler.h"
#include "defines.h"

static const QString VATBOOK_URL = "http://vatbook.euroutepro.com/servinfo.asp";
static const int REFRESH_INTERVAL = 15 * 60000;

VatbookHandler::VatbookHandler(QObject* _parent) : 
    QObject(_parent),
    __httpHandler(new HttpHandler()) {
  __bookings.insert("ZZZZ", new BookedAtcTableModel);
  
  connect(__httpHandler,    SIGNAL(finished(const QString&)),
          this,             SLOT(__dataFetched(const QString&)));
  
  connect(&__timer,         SIGNAL(timeout()),
          this,             SLOT(__timeToUpdate()));
  
  __timeToUpdate();
}

VatbookHandler::~VatbookHandler() {
  __clear();
  delete __bookings["ZZZZ"];
  delete __httpHandler;
}

void
VatbookHandler::__clear() {
  for (auto it = __bookings.begin(); it != __bookings.end(); ++it) {
    if (it.key() != "ZZZZ") {
      delete it.value();
      __bookings.remove(it.key());
    }
  }
}

void
VatbookHandler::__parseData(const QString& _data) {
  bool clientsSection = false;
  
  for (QString& line: _data.split('\n', QString::SkipEmptyParts)) {
    if (line.startsWith(';'))
      continue;
    
    if (line.startsWith('!')) {
      if (line.simplified() == "!CLIENTS:")
        clientsSection = true;
      else
        clientsSection = false;
      
      continue;
    }
    
    if (!clientsSection)
      continue;
    
    QStringList data = line.split(':');
    if (data[3] == "ATC") {
      BookedController* bc = new BookedController(data);
      if (!__bookings.contains(bc->getIcao()))
        __bookings.insert(bc->getIcao(), new BookedAtcTableModel);
      __bookings[bc->getIcao()]->addStaff(bc);
    }
  }
}

void
VatbookHandler::__dataFetched(const QString& _data) {
  if (_data.isEmpty())
    return;
  
  __clear();
  __parseData(_data);
  
  __timer.start(REFRESH_INTERVAL);
}

void
VatbookHandler::__handleError() {
  /* We actually should do nothing here - vatbook data should be
   * downloaded silently, without user interrupting.
   */
  __timer.start(REFRESH_INTERVAL);
}

void
VatbookHandler::__timeToUpdate() {
  __httpHandler->fetchData(VATBOOK_URL);
}

