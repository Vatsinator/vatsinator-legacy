/*
 * firarea.h
 * Copyright (C) 2015 Michał Garapich <michal@garapich.pl>
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

#ifndef GUI_FIRAREA_H
#define GUI_FIRAREA_H

#include "core/fir.h"
#include "core/lonlat.h"
#include "gui/maparea.h"
#include "gui/vtrgui_export.h"

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 * 
 * The \c FirArea draws FIRs on the map.
 */
class VTRGUI_EXPORT FirArea : public MapArea {
    Q_OBJECT

public:
    /**
     * Creates new FirArea. Passes \c scene and \c parent to \c MapArea.
     */
    explicit FirArea(Core::Fir* fir, Core::Fir::Boundaries boundaries,
                     MapScene* scene, QObject* parent = nullptr);
    
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
    
    const Core::Fir* fir() const { return m_fir; }
    Core::Fir* fir() { return m_fir; }
    
private slots:
    /**
     * Picks color and visibility.
     */
    void refreshStatus();
    
private:
    Core::Fir* m_fir;
    mutable QRectF m_boundingRect;
    QVector<Core::LonLat> m_polygon;
    QVector<Core::LonLat> m_boundary;
    bool m_isStaffed;
    QColor m_fillColor;

}; /** @} */

}} /* namespace Vatsinator::Gui */

#endif // GUI_FIRAREA_H
