/*
 * mapscene.h
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

#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QObject>
#include <QVector>

class FirItem;

class MapScene : public QObject {
  
  /**
   * The MapScene class is responsible for managing all the map items.
   */
  
  Q_OBJECT

public:
  explicit MapScene(QObject* parent = 0);
  
  inline const QVector<const FirItem*>
  firItems() const {
    return __firItems;
  }
  
  inline const QVector<const FirItem*>
  staffedFirItems() const {
    return __staffedFirItems;
  }
  
  inline const QVector<const FirItem*>
  unstaffedFirItems() const {
    return __unstaffedFirItems;
  }
  
private slots:
  void __updateData();
  
private:
  
  /* Groupped fir items */
  QVector<const FirItem*> __firItems;
  QVector<const FirItem*> __staffedFirItems;
  QVector<const FirItem*> __unstaffedFirItems;
  
  
};

#endif // MAPSCENE_H
