/*
 * mapinfowidget.h
 * Copyright (C) 2015  Michal Garapich michal@garapich.pl
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

#ifndef MAPINFOWIDGET_H
#define MAPINFOWIDGET_H

#include <QWidget>
#include <QDateTime>
#include "vatsimdata/lonlat.h"

class QLabel;

/**
 * MapInfoWidget encapsulates information displayed over the map.
 */
class MapInfoWidget : public QWidget
{
    Q_OBJECT
    
    /**
     * The currently shown position of the mouse cursor.
     */
    Q_PROPERTY(LonLat position READ position WRITE setPosition)
    
    /**
     * The date and time of the last update, in UTC.
     */
    Q_PROPERTY(QDateTime updated READ updated WRITE setUpdated)
    
    /**
     * Specifies whether the \c updated label is shown or not.
     * If not, the "Updating..." label is shown instead.
     */
    Q_PROPERTY(bool updatedVisible READ updatedVisible WRITE setUpdatedVisible)

public:
    MapInfoWidget(QWidget* parent = nullptr);
    
    void setPosition(const LonLat& position);
    void setUpdated(const QDateTime& updated);
    void setUpdatedVisible(bool updatedVisible);
    
    inline const LonLat& position() const
    {
        return __position;
    }
    
    inline const QDateTime& updated() const
    {
        return __updated;
    }
    
    inline bool updatedVisible() const
    {
        return __updatedVisible;
    }
    
private:
    LonLat __position;
    QLabel* __labelPosition;
    
    QDateTime __updated;
    QLabel* __labelUpdated;
    
    bool __updatedVisible;
    QLabel* __labelUpdating; /**< Visible only if __labelUpdated is not */
    
};

#endif // MAPINFOWIDGET_H
