/*
 * screenimageprovider.h
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

#ifndef SCREENIMAGEPROVIDER_H
#define SCREENIMAGEPROVIDER_H

#include <QQuickImageProvider>

/**
 * The ScreenImageProvider provides a currently rendered state of the screen.
 */
class ScreenImageProvider : public QQuickImageProvider {
    
public:
    ScreenImageProvider();
    
    /**
     * \copydoc QQuickImageProvider::requestImage()
     * 
     * Provided images:
     *  * "map" - the currently rendered map, in the current state.
     */
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;
    
};

#endif // SCREENIMAGEPROVIDER_H
