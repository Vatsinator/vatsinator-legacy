/*
 * flightprogress.cpp
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#include "flightprogress.h"
#include <QtGui>

namespace Vatsinator { namespace Imports {

QString generalizedDensity()
{
    static QString gd;
    if (gd.isEmpty()) {
        qreal dp = qApp->primaryScreen()->devicePixelRatio();
        if (dp <= 1.0)
            gd = QStringLiteral("mdpi");
        else if (dp <= 1.5)
            gd = QStringLiteral("hdpi");
        else if (dp <= 2.0)
            gd = QStringLiteral("xhdpi");
        else if (dp <= 3.0)
            gd = QStringLiteral("xxhdpi");
        else
            gd = QStringLiteral("xxxhdpi");
    }
    return gd;
}

FlightProgress::FlightProgress(QQuickItem* parent) :
    QQuickPaintedItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
    setAntialiasing(true);
    
    setImplicitHeight(24 * qApp->primaryScreen()->devicePixelRatio());
}

FlightProgress::~FlightProgress() {}

void FlightProgress::paint(QPainter* painter)
{
    QPixmap px;
    if (!QPixmapCache::find(QStringLiteral("flightmark_colorized"), &px)) {
        QPixmap orig(":/pixmaps/" + generalizedDensity() + "/flightmark_black_24dp.png");
        px = QPixmap(orig.size());
        px.fill(color());
        px.setMask(orig.createMaskFromColor(Qt::transparent));
        
        QPixmapCache::insert(QStringLiteral("flightmark_colorized"), px);
    }
    
    int dim = height() / 2;
    QPoint p1(0, dim);
    QPoint p2(width() * progress() * 0.01, dim);
    QPoint p3(width(), dim);
    QPoint offset(px.width() / qApp->primaryScreen()->devicePixelRatio() / 2 + 1, 0);
    
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    
    painter->setPen(color());
    
    if (progress() > 5)
        painter->drawLine(p1, p2 - offset);
    
    QColor lighter = color();
    lighter.setAlpha(50);
    painter->setPen(lighter);
    painter->drawLine(p2 + offset, p3);
    
    QRect rect = px.rect();
    rect.setSize(rect.size() / qApp->primaryScreen()->devicePixelRatio());
    rect.moveCenter(p2);
    painter->drawPixmap(rect, px, px.rect());
    
    painter->restore();
}

void FlightProgress::setProgress(int progress)
{
    if (m_progress != progress) {
        m_progress = progress;
        emit progressChanged(m_progress);
    }
}

void FlightProgress::setColor(const QColor& color)
{
    if (m_color != color) {
        m_color = color;
        emit colorChanged(m_color);
        update();
    }
}

}} /* namespace Vatsinator::Imports */
