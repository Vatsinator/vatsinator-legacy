/*
 * airportdetailswindow.h
 * Copyright (C) 2012  Michał Garapich <michal@garapich.pl>
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

#ifndef WIDGETS_AIRPORTDETAILSWINDOW_H
#define WIDGETS_AIRPORTDETAILSWINDOW_H

#include "core/airport.h"
#include "widgets/vtrwidgets_export.h"
#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>

namespace Ui { class AirportDetailsWindow; }

namespace Vatsinator { namespace Widgets {

/**
 * \ingroup Widgets
 * @{
 * 
 * A window for displaying airport details.
 */
class VTRWIDGETS_EXPORT AirportDetailsWindow : public QWidget {
    Q_OBJECT
    
    /**
     * The \c airport property keeps the airport of which details
     * are displayed in the window.
     */
    Q_PROPERTY(const Vatsinator::Core::Airport* airport READ airport WRITE setAirport)
    
signals:
    /**
     * Emitted when a client's details are requested.
     * \param callsign The requested client's callsign.
     */
    void clientDetailsRequested(const QString& callsign);
    
    /**
     * Emitted when user wants to show the airport on the map.
     */
    void showOnMapRequested();
    
public:
    /**
     * Creates a new AirportDetailsWindow. Passes \c parent to the
     * QWidget's constructor.
     */
    explicit AirportDetailsWindow(QWidget* parent = nullptr);
    
    /**
     * Destroys the AirportDetailsWindow.
     */
    virtual ~AirportDetailsWindow();
    
    const Core::Airport* airport() const { return m_airport; }
    void setAirport(const Core::Airport* airport);

protected:
    /**
     * \copydoc QWidget::showEvent()
     */
    void showEvent(QShowEvent* event) override;
    
    /**
     * \copydoc QWidget::changeEvent()
     */
    void changeEvent(QEvent* event) override;
    
    /**
     * \copydoc QWidget::resizeEvent()
     */
    void resizeEvent(QResizeEvent* event) override;
    
private slots:
    void showDetails(const QModelIndex& index);
    void updateMetar();
    void updateNotams();
    void updateBookings();
    
private:
    QScopedPointer<Ui::AirportDetailsWindow> ui;
    const Core::Airport* m_airport = nullptr;
    QMetaObject::Connection m_connection;

}; /** @} */

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_AIRPORTDETAILSWINDOW_H
