/*
 * screenimageprovider.cpp
 * Copyright (C) 2015  Michal Garapich <michal@garapich.pl>
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

#include <QtQuick>

#include "ui/quick/map.h"
#include "ui/quickuserinterface.h"

#include "screenimageprovider.h"

ScreenImageProvider::ScreenImageProvider() : QQuickImageProvider(QQmlImageProviderBase::Image)
{

}

QImage
ScreenImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    if (id == QStringLiteral("map")) {
        Map* map = qobject_cast<Map*>(qui()->findObjectByName("map"));
        
        if (!map) {
            if (size)
                *size = QSize(0, 0);
            
            return QImage();
        }
        
        QImage image = map->grab();
        if (image.isNull()) {
            qWarning("Could not grab the image");
        }
        
        if (size)
            *size = image.size();
        
        if (requestedSize.isValid()) {
            image = image.scaled(requestedSize);
        }
        
        return image;
    }
    
    return QImage();
}

