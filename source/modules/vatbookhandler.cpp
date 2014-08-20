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

#include <QtCore>

#include "modules/models/bookedatctablemodel.h"
#include "modules/vatbook/bookedcontroller.h"

#include "network/plaintextdownloader.h"

#include "vatbookhandler.h"

static const QString VatbookUrl = "http://vatbook.euroutepro.com/servinfo.asp";
static const int RefreshInterval = 15 * 60000; // 15 min

namespace {
  BookedAtcTableModel* emptyBookedAtcTable = new BookedAtcTableModel();
}

VatbookHandler::VatbookHandler(QObject* _parent) : 
    QObject(_parent),
    __downloader(new PlainTextDownloader(nullptr, this)) {
  
  connect(__downloader,    SIGNAL(finished(const QString&)),
          this,             SLOT(__dataFetched(const QString&)));
  connect(&__timer,         SIGNAL(timeout()),
          this,             SLOT(__timeToUpdate()));
  
  __timeToUpdate();
}

VatbookHandler::~VatbookHandler() {
  __clear();
  
  delete emptyBookedAtcTable;
}

BookedAtcTableModel*
VatbookHandler::model(const QString& _icao) {
  return __bookings.value(_icao, nullptr);
}

BookedAtcTableModel*
VatbookHandler::notNullModel(const QString& _icao) {
   return __bookings.value(_icao, emptyBookedAtcTable);
}

void
VatbookHandler::__clear() {   
  qDeleteAll(__bookings);
  
  __bookings.clear();
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
      if (!__bookings.contains(bc->icao()))
        __bookings.insert(bc->icao(), new BookedAtcTableModel());
      __bookings[bc->icao()]->add(bc);
    }
  }
}

void
VatbookHandler::__dataFetched(const QString& _data) {
  if (_data.isEmpty())
    return;
  
  __clear();
  __parseData(_data);
  
  __timer.start(RefreshInterval);
}

void
VatbookHandler::__handleError() {
  /* We actually should do nothing here - vatbook data should be
   * downloaded silently, without interrupting the user.
   */
  __timer.start(RefreshInterval);
}

void
VatbookHandler::__timeToUpdate() {
  __downloader->fetchData(VatbookUrl);
}

