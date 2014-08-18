/*
    modelmatcher.h
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


#ifndef MODELMATCHER_H
#define MODELMATCHER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QPair>
#include <QList>

#include "ui/notifiable.h"

class QOpenGLTexture;

/**
 * This class is responsible for matching user airplanes to 
 * corresponding models.
 */
class ModelMatcher : public QObject, public Notifiable {
  Q_OBJECT

public:
  /**
   * Reads the models.dat file.
   */
  ModelMatcher(QObject* = nullptr);
  
  virtual ~ModelMatcher();

  /**
   * @param acft Aircraft code that comes from the flight plan.
   * @return Model's texture ID.
   */
  QOpenGLTexture* matchMyModel(const QString&) const;
  
private:
  void __readModels();
  void __loadPixmaps();

private:
  /*
   * Aircraft <-> id pairs
   */
  QList<QPair<QString, QString>> __modelsIds;
  
  /*
   * Id <-> texture pairs
   */
  QMap<QString, QOpenGLTexture*> __modelsPixmaps;

};

#endif // MODELMATCHER_H
