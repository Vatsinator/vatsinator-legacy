/*
 * tmaarea.h
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

#ifndef GUI_TMAAREA_H
#define GUI_TMAAREA_H

#include "core/airportobject.h"
#include "gui/maparea.h"
#include "gui/vtrgui_export.h"

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 * 
 * Handles drawing TMAs.
 */
class VTRGUI_EXPORT TmaArea : public MapArea {
    Q_OBJECT

public:
    /**
     * Creates a new TmaArea instance.
     * 
     * \param airport The airport around which TMA is being drawn.
     * \param boundary List of points that make the TMA boundary. If this list
     *  is empty, a default circle is drawn instead.
     * \param scene The parent \c MapScene instance.
     */
    explicit TmaArea(Core::AirportObject* airport, QList<Core::LonLat> boundary, MapScene* scene,
                     QObject* parent = nullptr);
    
    /**
     * \copydoc MapArea::boundingRect()
     */
    virtual QRectF boundingRect() const override;
    
    /**
     * \copydoc MapDrawable::isVisible()
     */
    virtual bool isVisible() const override;
    
    /**
     * \copydoc MapDrawable::draw(QPainter*, const WorldTransform&, DrawFlags)
     */
    virtual void draw(WorldPainter* painter, DrawFlags flags) const override;
    
private slots:
    void refreshVisible();
    
private:
    Core::AirportObject* m_airport;
    QRectF m_boundingRect;
    bool m_visible;
    QList<Core::LonLat> m_boundary;

}; /** @} */

}} /* namespace Vatsinator::Gui */

#endif // GUI_TMAAREA_H
