/*
 * flightitem.h
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

#ifndef GUI_FLIGHTITEM_H
#define GUI_FLIGHTITEM_H

#include "guiexport.h"
#include "mapitem.h"
#include "core/pilot.h"
#include <QPixmap>

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 * 
 * The \c FlightItem draws a single flight on the map.
 */
class __VtrGuiApi__ FlightItem : public MapItem {
    Q_OBJECT
    
    /**
     * The \c pilot property keeps the flight that is being tracked.
     */
    Q_PROPERTY(Vatsinator::Core::Pilot* pilot READ pilot CONSTANT)

public:
    /**
     * The constructor takes \c pilot as a data provider of what to show
     * on the map. Data is updated automatically.
     */
    explicit FlightItem(Core::Pilot* pilot, MapScene* scene,
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
     * Gives direct access to the Pilot instance this item represents.
     */
    const Core::Pilot* pilot() const { return m_pilot; }
    Core::Pilot* pilot() { return m_pilot; }
    
    FlightItem() = delete;
    
private:
    void prepareModel() const;
    void prepareLabel() const;
    void fetchNodes() const;

    static void dropShadow(QPixmap* image);
    
private slots:
    /**
     * Removes the model pixmap, so it will be re-created with the next update.
     */
    void invalidateModel();
    
    /**
     * Clears flight nodes.
     */
    void clearNodes();
    
private:
    Core::Pilot* m_pilot;
    mutable QPixmap m_model;
    mutable QPixmap m_label;
    mutable QList<Core::LonLat> m_depToPilotNodes, m_pilotToDestNodes;
    
}; /** @} */

}} /* namespace Vatsinator::Gui */

Q_DECLARE_METATYPE(Vatsinator::Gui::FlightItem*)

#endif // GUI_FLIGHTITEM_H
