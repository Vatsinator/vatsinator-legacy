/*
    modelmatcher.cpp
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

#include <QtWidgets>
#include <QOpenGLTexture>

#include "glutils/glresourcemanager.h"
#include "ui/widgets/mapwidget.h"
#include "glutils/texture.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "storage/filemanager.h"
#include "vatsinatorapplication.h"

#include "modelmatcher.h"

ModelMatcher::ModelMatcher(QObject* _parent) : QObject(_parent) {
  __readModels();
  __loadPixmaps();
}

ModelMatcher::~ModelMatcher() {
  for (auto it: __modelsPixmaps) {
    delete it;
  }
}

QOpenGLTexture*
ModelMatcher::matchMyModel(const QString& _acft) const {
  if (_acft.isEmpty())
    return __modelsPixmaps["1p"];
  
  for (auto pair: __modelsIds) {
    if (_acft.contains(pair.first)) {
      return __modelsPixmaps[pair.second];
    }
  }
  
  return __modelsPixmaps["1p"];
}

void
ModelMatcher::__readModels() {
  __modelsIds << qMakePair(QString("ZZZZ"), QString("1p"));

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

    auto split = line.split(' ');
    __modelsIds << qMakePair(split[0], split[1]);
  }
}

void
ModelMatcher::__loadPixmaps() {
  for (auto pair: __modelsIds) {
    if (!__modelsPixmaps.contains(pair.second)) {
      QString mPath = ":/pixmaps/model_" % pair.second % "32.png";
      QImage model(mPath);
      Q_ASSERT(!model.isNull());
      QOpenGLTexture* t = new QOpenGLTexture(QOpenGLTexture::Target2D);
      __modelsPixmaps.insert(pair.second, t);
      t->setData(model.mirrored());
      t->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Nearest);
    }
  }
}
