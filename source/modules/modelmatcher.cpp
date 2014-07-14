/*
    modelmatcher.cpp
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

#include "glutils/glresourcemanager.h"
#include "glutils/texture.h"
#include "ui/userinterface.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "storage/filemanager.h"
#include "vatsinatorapplication.h"

#include "modelmatcher.h"

ModelMatcher::ModelMatcher(QObject* _parent) : QObject(_parent) {
  __readModels();
  __loadPixmaps();
}

ModelMatcher::~ModelMatcher() {
  qDeleteAll(__pixmaps);
}

const Texture*
ModelMatcher::matchMyModel(const QString& _acft) const {
  if (_acft.isEmpty())
    return __modelsPixmaps["ZZZZ"];
  
  for (auto it = __modelsPixmaps.begin(); it != __modelsPixmaps.end(); ++it)
    if (_acft.contains(it.key(), Qt::CaseInsensitive))
      return it.value();

  return __modelsPixmaps["ZZZZ"];
}

void
ModelMatcher::__readModels() {
  __modelsFiles["ZZZZ"] = "1p"; // default

  QFile modelsFile(FileManager::path("data/model"));

  if (!modelsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    notifyWarning(tr("File %1 could not be opened! Check file permissions or reinstall the application.").arg(modelsFile.fileName()));
    return;
  }

  while (!modelsFile.atEnd()) {
    QString line(modelsFile.readLine());
    line = line.simplified();

    if (line.startsWith('#') || line.isEmpty())
      continue;

    auto splitted = line.split(' ');
    __modelsFiles.insert(splitted[0], splitted[1]);
  }
}

void
ModelMatcher::__loadPixmaps() {
  QMap<QString, Texture*> pixmapsLoaded;
  QString path(FileManager::staticPath(FileManager::Pixmaps));
  QList<QString> models = { "1p", "2p", "4p", "2j", "3j", "4j", "conc" };
  
  for (const QString& s: models) {
    QString mPath = path % QDir::separator() % s % "32.png";
    Texture* t = new Texture(mPath);
    __pixmaps << t;
    pixmapsLoaded.insert(s, t);
  }

  for (auto it = __modelsFiles.begin(); it != __modelsFiles.end(); ++it) {
    Q_ASSERT(pixmapsLoaded.contains(it.value()));
    __modelsPixmaps.insert(it.key(), pixmapsLoaded[it.value()]);
  }
}
