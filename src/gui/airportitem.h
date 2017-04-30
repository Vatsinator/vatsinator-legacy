/*
 * airportitem.h
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

#ifndef GUI_AIRPORTITEM_H
#define GUI_AIRPORTITEM_H

#include "core/airportobject.h"
#include "gui/mapitem.h"
#include "gui/vtrgui_export.h"

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 * 
 * The \c AirportItem represents a single airport on the map.
 */
class VTRGUI_EXPORT AirportItem : public MapItem {
    Q_OBJECT
    
    /**
     * The \c airport property keeps the airport that is being tracked.
     */
    Q_PROPERTY(Vatsinator::Core::AirportObject* airport READ airport CONSTANT)

public:
    /**
     * Creates a new AirportItem.
     */
    explicit AirportItem(Core::AirportObject* airport, MapScene* scene,
                         QObject* parent = nullptr);
    
    /**
     * \copydoc MapItem::isVisible()
     */
    bool isVisible() const override;
    
    /**
     * \copydoc MapItem::position()
     */
    Core::LonLat position() const override;
    
    /**
     * \copydoc MapItem::size()
     */
    QSize size() const override;
    
    /**
     * \copydoc MapItem::draw()
     */
    void draw(WorldPainter* painter, DrawFlags flags) const override;
    
    /**
     * \copydoc MapItem::z()
     */
    int z() const override;
    
    /**
     * Gives access to the airport object.
     */
    const Core::AirportObject* airport() const { return m_airport; }
    Core::AirportObject* airport() { return m_airport; }
    
    AirportItem() = delete;
    
private:
    void prepareIcon() const;
    void prepareLabel() const;
    
private slots:
    /**
     * Removes the airport icon so it gets re-loaded on the next repaint.
     */
    void removeIcon();
    
private:
    Core::AirportObject* m_airport;
    mutable QPixmap m_icon;
    mutable QPixmap m_label;
    
}; /** @} */

}} /* namespace Vatsinator::Gui */

Q_DECLARE_METATYPE(Vatsinator::Gui::AirportItem*)

#endif // GUI_AIRPORTITEM_H
