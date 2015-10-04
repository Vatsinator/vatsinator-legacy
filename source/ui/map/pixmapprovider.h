/*
 * pixmapprovider.h
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

#ifndef PIXMAPPROVIDER_H
#define PIXMAPPROVIDER_H

#include <QObject>
#include <QPixmap>
#include <QImage>
#include <QMap>

/**
 * The PixmapProvider class is respoinsble for generating and storing
 * pixmaps that all map items can re-use.
 */
class PixmapProvider : public QObject {
    Q_OBJECT

public:
    explicit PixmapProvider(QObject *parent = nullptr);
    
    /**
     * Background for airport labels.
     */
    QPixmap backgroundForAirportLabel();
    
    /**
     * Background for flight labels.
     * 
     * \param targetRect Where the text should be rendered.
     */
    QPixmap backgroundForFlightLabel(QRect* targetRect);
    
    /**
     * Matches the given model and returns the best representing
     * pixmap.
     */
    QPixmap pixmapForModel(const QString& modelString);
    
private:
    void __readModelMappings();
    QString __matchModel(const QString& modelString);
    QImage __modelColorized(const QImage& image, const QColor& color);
    
    QMap<QString, QString> __mappings;
    QRect __flightLabelRect;
    
};

#endif // PIXMAPPROVIDER_H
