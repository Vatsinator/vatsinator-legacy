/*
    updatechecker.cpp
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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

#include "network/plaintextdownloader.h"

#include "ui/widgets/newversionnotificationwidget.h"

#include "vatsinatorapplication.h"

#include "updatechecker.h"
#include "defines.h"

// new version will be fetched after 3 seconds
static const int FETCH_DELAY = 3 * 1000;

UpdateChecker::UpdateChecker() :
    QObject(),
    __currentVersion(VATSINATOR_VERSION),
    __httpHandler(new PlainTextDownloader()),
    __notification(nullptr) {
  connect(__httpHandler,    SIGNAL(finished(const QString&)),
          this,             SLOT(__parseVersion(const QString&)));
}

UpdateChecker::~UpdateChecker() {
  if (__notification)
    delete __notification;
  
  delete __httpHandler;
}

void
UpdateChecker::init() {
  QTimer::singleShot(FETCH_DELAY, this, SLOT(__fetchVersion()));
}

UpdateChecker::Version::Version(const QString& _version) {
  __major = _version.section('.', 0, 0).toInt();
  
  if (_version.contains('-')) {
    __minor = _version.section('.', 1).section('-', 0, 0).toInt();
  } else {
    __minor = _version.section('.', 1).toInt();
  }
  
  VatsinatorApplication::log("Parsed %s as major=%i, minor=%i",
                             qPrintable(_version), __major, __minor);
}

UpdateChecker::Version::Version() :
   __major(-1),
   __minor(-1) {}

bool
UpdateChecker::Version::operator <(const UpdateChecker::Version& _other) {
  if (__major < _other.__major)
    return true;
  
  if (__major == _other.__major && __minor < _other.__minor)
    return true;
  
  return false;
}

void
UpdateChecker::__fetchVersion() {
  __httpHandler->fetchData(
      static_cast<QString>(VATSINATOR_REPO_URL) %
      "/VERSION");
}

void
UpdateChecker::__parseVersion(const QString& _versionString) {
  __newVersion = Version(_versionString.simplified());
  
  if (__currentVersion < __newVersion) {
    __notification = new NewVersionNotificationWidget();
    __notification->show();
  }
}



