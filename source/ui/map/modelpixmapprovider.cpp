/*
 * modelpixmapprovider.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#include "modelpixmapprovider.h"

ModelPixmapProvider::ModelPixmapProvider(QObject* parent) : QObject (parent)
{
    
}

QPixmap
ModelPixmapProvider::pixmapForModel(const QString& modelString)
{
    /* Model matching will be here */
    Q_UNUSED(modelString);
    
    QString model = "B747";
    QPixmap px;
    if (!QPixmapCache::find(model, &px)) {
        px = __prepareModelPixmap(model);
    }
    
    return px;
}

QPixmap
ModelPixmapProvider::__prepareModelPixmap(const QString& model)
{
    QPixmap px;
    if (QPixmapCache::find(model, &px))
        return px;
    
    QString path = ":/pixmaps/mdpi/" + model + ".png";
    QImage image(path);
    Q_ASSERT(!image.isNull());
    
    /* Fill the white color with the outline color */
    QBitmap mask = QBitmap::fromImage(image.createAlphaMask());
    QPainter p(&image);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    p.setPen(QColor(238, 220, 122));
    p.drawPixmap(image.rect(), mask, mask.rect());
    
    /* I don't know why this useful function isn't declared somewhere, really */
    extern QPainterPath qt_regionToPath(const QRegion &region);
    QPainterPath modelPath = qt_regionToPath(QRegion(mask));
    
    QPen outlinePen(QColor(151, 145, 80));
    outlinePen.setWidthF(0.5);
    p.setPen(outlinePen);
    p.setOpacity(0.5);
    p.drawPath(modelPath);
    
    p.end();
    
    px.convertFromImage(image);
    QPixmapCache::insert(model, px);
    return px;
}
