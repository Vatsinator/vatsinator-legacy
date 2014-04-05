/*
    modelmatcher.h
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


#ifndef MODELMATCHER_H
#define MODELMATCHER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QtOpenGL>

#include "singleton.h"

class ModelMatcher : public QObject, public Singleton<ModelMatcher> {
  
  /**
   * This class matches the models to the planes.
   */
  
  Q_OBJECT
  
signals:
  void warning(QString);

public:
  /**
   * Reads the models.dat file.
   */
  ModelMatcher(QObject* = nullptr);

  /**
   * @param acft Aircraft code that comes from the flight plan.
   * @return Model's texture ID.
   */
  GLuint matchMyModel(const QString&);
  
private:
  void __readModels();
  
private slots:
  void __loadPixmaps();

private:

  QMap<QString, QString> __modelsFiles;
  QMap<QString, GLuint>  __modelsPixmaps;


};

#endif // MODELMATCHER_H
