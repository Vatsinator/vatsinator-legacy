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

#ifndef GUI_PIXMAPPROVIDER_H
#define GUI_PIXMAPPROVIDER_H

#include "gui/vtrgui_export.h"
#include <QtCore/QObject>
#include <QtGui/QPixmap>

namespace Vatsinator { namespace Gui {

class MapScene;
class ModelMatcher;

/**
 * \ingroup Gui
 * @{
 * 
 * The PixmapProvider class is respoinsble for generating and storing
 * pixmaps that all map items can re-use.
 */
class VTRGUI_EXPORT PixmapProvider : public QObject {
    Q_OBJECT
    
public:
    explicit PixmapProvider(QObject* parent = nullptr);
    
    virtual ~PixmapProvider();
    
    /**
     * Matches the given model and returns the best pixmap.
     */
    QPixmap pixmapForModel(const QString& modelString) const;
    
    /**
     * Returns the airport pixmap.
     */
    QPixmap pixmapForAirport(bool staffed) const;

    /**
     * Returns the background for label.
     */
    QPixmap flightTooltipBackground() const;

    /**
     * Returns the background for airport label.
     */
    QPixmap airportLabelBackground(const QFont& font) const;
    
protected:
    void setModelMatcher(const ModelMatcher* modelMatcher);
    
private:
    friend class MapScene;

    QString matchModel(const QString& modelString) const;
    QImage modelColorized(const QImage& image, const QColor& color) const;
    
    ModelMatcher* m_defaultModelMatcher;
    const ModelMatcher* m_modelMatcher = nullptr;
    
}; /** @} */

}} /* namespace Vatsinator::Gui */

#endif // GUI_PIXMAPPROVIDER_H
