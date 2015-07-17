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
#include "ui/models/atctablemodel.h"
#include "ui/userinterface.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "firitem.h"

FirItem::FirItem(const Fir* fir, QObject* parent) :
    MapItem(parent),
    __scene(qobject_cast<MapScene * >(parent)),
    __fir(fir),
    __position(fir->data()->header.textPosition.x, fir->data()->header.textPosition.y)
{
    
}

FirItem::~FirItem()
{
    
}

bool
FirItem::isVisible() const
{
    if (position().isNull())
        return false;
        
    if (data()->isEmpty())
        return __scene->settings().view.unstaffed_firs;
    else
        return __scene->settings().view.staffed_firs;
}

bool
FirItem::isLabelVisible() const
{
    if (data()->isEmpty())
        return __scene->settings().view.unstaffed_firs;
    else
        return __scene->settings().view.staffed_firs;
}

const LonLat&
FirItem::position() const
{
    return __position;
}

void
FirItem::draw(QPainter* painter, const QTransform& transform) const
{

}

QString
FirItem::tooltipText() const
{
    QString desc;
    
    if (!data()->name().isEmpty()) {
        desc.append(data()->name());
        
        if (!data()->country().isEmpty()) {
            desc.append(", ");
            desc.append(data()->country());
        }
    }
    
    QString staff;
    
    for (const Controller* c : data()->staff()->staff()) {
        staff.append("<br>");
        staff.append(QString("%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
    }
    
    for (const Controller* c : data()->uirStaff()->staff()) {
        staff.append("<br>");
        staff.append(QString("%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
    }
    
    if (desc.isEmpty() && staff.isEmpty())
        return QString();
    else
        return QString("<p style='white-space:nowrap'><center>") % desc % staff % QString("</center></p>");
}

void
FirItem::showDetails() const
{
    vApp()->userInterface()->showDetails(data());
}
