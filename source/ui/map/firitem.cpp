/*
 * firitem.cpp
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

#include <QtGui>

#include "db/firdatabase.h"
#include "storage/settingsmanager.h"
#include "ui/map/mapconfig.h"
#include "ui/map/mapscene.h"
#include "ui/userinterface.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "firitem.h"

FirItem::FirItem(const Fir* fir, QObject* parent) :
    MapItem(parent),
    __scene(qobject_cast<MapScene*>(parent)),
    __fir(fir),
    __position(fir->labelPosition())
{
    QString labelText = __fir->icao();
    if (__fir->isOceanic()) {
        labelText += " Oceanic";
    }
    
    __label.setText(labelText);
    
    connect(vApp()->settingsManager(), &SettingsManager::settingsChanged, this, &FirItem::__prepareLabel);
    __prepareLabel();
}

bool
FirItem::isVisible() const
{
    if (position().isNull())
        return false;
        
    return data()->isEmpty() ? __scene->settings().view.unstaffed_firs : __scene->settings().view.staffed_firs;
}

LonLat
FirItem::position() const
{
    return __position;
}

void
FirItem::draw(QPainter* painter, const WorldTransform& transform, DrawFlags flags) const
{
    QPen origPen = painter->pen();
    QFont origFont = painter->font();
    
    QPen pen(data()->isStaffed() ?
            (flags & DrawSelected ? QColor(137, 66, 66) : QColor(157, 86, 86)) :
            (flags & DrawSelected ? QColor(135, 135, 135) : QColor(155, 155, 155)));
    pen.setWidth(3);
    painter->setPen(pen);
    painter->setFont(__font);
    
    QRect rect(QPoint(0, 0), __label.size().toSize());
    rect.moveCenter(position() * transform);
    
    painter->drawStaticText(rect.topLeft(), __label);
    painter->setPen(origPen);
    painter->setFont(origFont);
}

int
FirItem::z() const
{
    return 1;
}

void
FirItem::__prepareLabel()
{
    __font = SM::get("map.fir_font").value<QFont>();
    __label.prepare(QTransform(), __font);
}
