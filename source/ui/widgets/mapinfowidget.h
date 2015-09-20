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
class QGraphicsDropShadowEffect;

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
    
    /**
     * Number of connected clients.
     */
    Q_PROPERTY(int clients READ clients WRITE setClients)
    
    /**
     * Number of connected pilots.
     */
    Q_PROPERTY(int pilots READ pilots WRITE setPilots)
    
    /**
     * Number of connected ATCs.
     */
    Q_PROPERTY(int atcs READ atcs WRITE setAtcs)
    
    /**
     * Number of connected observers.
     */
    Q_PROPERTY(int observers READ observers WRITE setObservers)

public:
    MapInfoWidget(QWidget* parent = nullptr);
    
    void setPosition(const LonLat& position);
    void setUpdated(const QDateTime& updated);
    void setUpdatedVisible(bool updatedVisible);
    void setClients(int clients);
    void setPilots(int pilots);
    void setAtcs(int atcs);
    void setObservers(int observers);
    
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
    
    inline int clients() const
    {
        return __clients;
    }
    
    inline int pilots() const
    {
        return __pilots;
    }
    
    inline int atcs() const
    {
        return __atcs;
    }
    
    inline int observers() const
    {
        return __observers;
    }
    
private:
    QGraphicsDropShadowEffect* __prepareEffect();
    void __updateStatusLabel();
    
    LonLat __position;
    
    QDateTime __updated;
    QLabel* __labelUpdated;
    
    bool __updatedVisible;
    QLabel* __labelUpdating; /**< Visible only if __labelUpdated is not */
    
    int __clients;
    int __pilots;
    int __atcs;
    int __observers;
    QLabel* __labelStatus;
    
};

#endif // MAPINFOWIDGET_H
