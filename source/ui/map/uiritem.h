/*
 * uiritem.h
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

#ifndef UIRITEM_H
#define UIRITEM_H

#include <QObject>
#include <QList>
#include "ui/map/mapitem.h"

class FirItem;
class Uir;

class UirItem : public QObject, public MapItem {
  Q_OBJECT

public:
  UirItem(const Uir*, QObject* = nullptr);
  
  bool isVisible() const override;
  bool isLabelVisible() const override;
  void showDetails() const override;
  QString tooltipText() const override;
  const LonLat& position() const override;
  void drawItem(QOpenGLShaderProgram*) const override;
  void drawLabel(QOpenGLShaderProgram*) const override;
  
  inline const Uir* data() const { return __uir; }
  inline const QList<FirItem*>& firItems() const {
    return __firItems;
  }
  
private:
  const Uir* __uir;
  QList<FirItem*> __firItems;

};

#endif // UIRITEM_H
