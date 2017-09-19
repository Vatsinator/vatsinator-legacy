/*
 * mapdrawable.h
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

#ifndef GUI_MAPDRAWABLE_H
#define GUI_MAPDRAWABLE_H

#include "gui/worldpainter.h"
#include "gui/v8rgui_export.h"
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtGui/QPainter>

namespace Vatsinator { namespace Gui {

class MapScene;

/**
 * \ingroup Gui
 * @{
 * 
 * The \c MapDrawable is a base class for everything that can be rendered
 * on the map.
 */
class V8RGUI_EXPORT MapDrawable : public QObject {
    Q_OBJECT
    Q_ENUMS(DrawFlag)
    
signals:
    /**
     * Emitted when the item needs to be re-rendered.
     */
    void updated();

public:
    /**
     * Flags used during the rendering process.
     */
    enum DrawFlag {
        DrawSelected    = 0x1 /**< Indicates that the item is focused */,
        Debug           = 0x2 /**< Draw extra info on the map. This flag is
                                   enabled by MapRenderer if environment
                                   variable VTR_MAP_DEBUG is set. */
    };
    Q_DECLARE_FLAGS(DrawFlags, DrawFlag)
    
    /**
     * The default constructor takes the scene it is rendered on.
     */
    explicit MapDrawable(MapScene* scene, QObject* parent = nullptr);
    
    /**
     * Destroys the drawable.
     */
    virtual ~MapDrawable();
    
    /**
     * Gives access to the scene the item is attached to.
     * @{
     */
    MapScene* scene() { return m_scene; }
    const MapScene* scene() const { return m_scene; }
    /** @} */
    
    /**
     * Indicates whether the drawable should be rendered or not.
     */
    virtual bool isVisible() const = 0;
    
    /**
     * Draws the item using the provided painter.
     */
    virtual void draw(WorldPainter* painter, DrawFlags flags) const = 0;
    
private:
    MapScene* m_scene;

}; /** @} */

}} /* namespace Vatsinator::Gui */

Q_DECLARE_METATYPE(Vatsinator::Gui::MapDrawable*)
Q_DECLARE_OPERATORS_FOR_FLAGS(Vatsinator::Gui::MapDrawable::DrawFlags)

#endif // GUI_MAPDRAWABLE_H
