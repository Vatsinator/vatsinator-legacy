/*
 * firitem.cpp
 * Copyright (C) 2014 Michał Garapich <michal@garapich.pl>
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

#include "firitem.h"
#include "mapscene.h"
#include "worldtransform.h"
#include "core/atc.h"
#include <QtGui>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Gui {

FirItem::FirItem(Fir* fir, MapScene* scene, QObject* parent) :
    MapItem(scene, parent),
    m_fir(fir)
{
    connect(m_fir, &ClientList::clientCountChanged, this, &FirItem::removeLabel);
    
    m_font.setWeight(QFont::DemiBold);
}

bool FirItem::isVisible() const
{
    return true;
}

LonLat FirItem::position() const
{
    return m_fir->labelPosition();
}

QSize FirItem::size() const
{
    if (m_label.isNull())
        loadLabel();

    Q_ASSERT(!m_label.isNull());
    return m_label.size();
}

void FirItem::draw(WorldPainter* painter, MapItem::DrawFlags flags) const
{
    Q_UNUSED(flags);

    if (m_label.isNull())
        loadLabel();

    Q_ASSERT(!m_label.isNull());
    painter->drawPixmap(position(), m_label);
}

int FirItem::z() const
{
    return 1;
}

void FirItem::loadLabel() const
{
    QString label = m_fir->icao();
    if (m_fir->isOceanic())
        label += " Oceanic";
    
    QFontMetrics fm(m_font);
    int w = fm.width(label);
    int h = fm.height();
    
    int dpr = static_cast<int>(qApp->primaryScreen()->devicePixelRatio());
    m_label = QPixmap(w * dpr, h * dpr);
    m_label.setDevicePixelRatio(dpr);
    m_label.fill(Qt::transparent);
    
    QPainter p(&m_label);
    
    auto atcs = m_fir->clients<Atc>();
    auto fss = std::count_if(atcs.begin(), atcs.end(), [](auto it) { return it->facility() == Atc::Ctr || it->facility() == Atc::Fss; });
    auto uirs = std::count_if(atcs.begin(), atcs.end(), [](auto it) { return it->isUir(); });

    QColor color;
    if (fss == 0)
        color = QColor(155, 155, 155);
    else if (fss > uirs)
        color = QColor(157, 86, 86);
    else if (fss == uirs)
        color = QColor(86, 120, 157);
    else
        Q_UNREACHABLE();

    QPen pen(color);
    p.setPen(pen);
    
    p.drawText(m_label.rect(), label);
}

void FirItem::removeLabel()
{
    m_label = QPixmap();
}

}} /* namespace Vatsinator::Gui */
