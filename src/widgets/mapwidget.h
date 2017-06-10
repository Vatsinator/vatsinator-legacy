/*
 * mapwidget.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#ifndef WIDGETS_MAPWIDGET_H
#define WIDGETS_MAPWIDGET_H

#include "core/airport.h"
#include "core/atc.h"
#include "core/fir.h"
#include "core/pilot.h"
#include "gui/maprenderer.h"
#include "widgets/vtrwidgets_export.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QWidget>
#include <QtCore/QScopedPointer>

namespace Vatsinator { namespace Widgets {

class MouseHelper;

/**
 * \ingroup Widgets
 * @{
 * 
 * The MapWidget encapsulates \c MapRenderer into a QWidget. 
 */
class VTRWIDGETS_EXPORT MapWidget : public QWidget {
    Q_OBJECT
    
    /**
     * Keeps the running renderer instance.
     */
    Q_PROPERTY(Vatsinator::Gui::MapRenderer* renderer READ renderer WRITE setRenderer)
    
signals:
    /**
     * Emitted when user clicks an item on the map.
     */
    void mapItemClicked(const Gui::MapItem* item);
    
    /**
     * Emitted when user requested airport details.
     */
    void airportDetailsRequested(const Core::Airport* airport);
    
    /**
     * Emitted when user requested ATC details.
     */
    void atcDetailsRequested(const Core::Atc* atc);
    
    /**
     * Emitted when user requested flight details.
     */
    void flightDetailsRequested(const Core::Pilot* flight);
    
    /**
     * Emitted when user requested FIR details.
     */
    void firDetailsRequested(const Core::Fir* fir);

    /**
     * Emitted when user requested a weather report.
     */
    void metarRequested(QString icao);

public:
    /**
     * The default constructor passes \c parent to the QWidget.
     */
    explicit MapWidget(QWidget* parent = nullptr);
    
    /**
     * Destroys the widget.
     */
    virtual ~MapWidget();
    
    /**
     * \copydoc QWidget::event(QEvent*)
     */
    bool event(QEvent* event) override;
    
    const Gui::MapRenderer* renderer() const { return m_renderer; }
    Gui::MapRenderer* renderer() { return m_renderer; }
    void setRenderer(Gui::MapRenderer* renderer);
    
protected:
    /**
     * \copydoc QWidget::paintEvent(QPaintEvent*)
     */
    void paintEvent(QPaintEvent* event) override;
    
    /**
     * \copydoc QWidget::resizeEvent(QResizeEvent*)
     */
    void resizeEvent(QResizeEvent* event) override;
    
    /**
     * \copydoc QWidget::wheelEvent(QWheelEvent*)
     */
    void wheelEvent(QWheelEvent* event) override;
    
    /**
     * \copydoc QWidget::mousePressEvent(QMouseEvent*)
     */
    void mousePressEvent(QMouseEvent* event) override;
    
    /**
     * \copydoc QWidget::mouseReleaseEvent(QMouseEvent*)
     */
    void mouseReleaseEvent(QMouseEvent* event) override;
    
    /**
     * \copydoc QWidget::mouseMoveEvent(QMouseEvent*)
     */
    void mouseMoveEvent(QMouseEvent* event) override;
    
    /**
     * \copydoc QWidget::contextMenuEvent(QContextMenuEvent*)
     */
    void contextMenuEvent(QContextMenuEvent* event) override;

    /**
     * \copydoc QWidget::keyPressEvent(QKeyEvent*)
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * \copydoc QWidget::keyReleaseEvent(QKeyEvent*)
     */
    void keyReleaseEvent(QKeyEvent* event) override;
    
private:
    /**
     * Returns item located under the point (in widget coordinates).
     */
    const Gui::MapItem* underPoint(const QPoint& widgetPosition) const;
    
    /**
     * Creates a submenu for the given \c point.
     */
    QMenu* makeMenu(const QPoint& pos);
    
    Gui::MapRenderer* m_renderer = nullptr; /**< Bound renderer instance */
    QScopedPointer<MouseHelper> m_mouse;
    bool m_selectAll = false;
    
}; /** @} */

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_MAPWIDGET_H
