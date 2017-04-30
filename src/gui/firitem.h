/*
 * firitem.h
 * Copyright (C) 2014 Michał Garapich <michal@garapich.pl>
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

#ifndef GUI_FIRITEM_H
#define GUI_FIRITEM_H

#include "core/firobject.h"
#include "gui/mapitem.h"
#include "gui/vtrgui_export.h"

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 * 
 * The \c FirItem represents a FIR label.
 */
class VTRGUI_EXPORT FirItem : public MapItem {
    Q_OBJECT
    
    /**
     * The \c fir property keeps the FIR of which label is being drawn.
     */
    Q_PROPERTY(Vatsinator::Core::FirObject* fir READ fir CONSTANT)
    
public:
    explicit FirItem(Core::FirObject* fir, MapScene* scene,
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
    
    const Core::FirObject* fir() const { return m_fir; }
    Core::FirObject* fir() { return m_fir; }
    
    FirItem() = delete;
    
private:
    void loadLabel() const;
    
private slots:
    void removeLabel();
    
private:
    Core::FirObject* m_fir;
    mutable QPixmap m_label;
    QFont m_font;

}; /** @} */

}} /* namespace Vatsinator::Gui */

#endif // GUI_FIRITEM_H
