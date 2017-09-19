/*
 * flightprogresswidget.h
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#ifndef WIDGETS_FLIGHTPROGRESSWIDGET_H
#define WIDGETS_FLIGHTPROGRESSWIDGET_H

#include "core/pilot.h"
#include "widgets/v8rwidgets_export.h"
#include <QtCore/QPointer>
#include <QtWidgets/QProgressBar>

namespace Vatsinator { namespace Widgets {

/**
 * \ingroup Widgets
 * @{
 * 
 * The FlightProgressWidget represents flight progress in a nice, graphical
 * form.
 */
class V8RWIDGETS_EXPORT FlightProgressWidget : public QProgressBar {
    Q_OBJECT
    
    /**
     * Keeps instance of the flight.
     */
    Q_PROPERTY(const Vatsinator::Core::Pilot* flight READ flight WRITE setFlight)

public:
    /**
     * Creates a new FlightProgressWidget, passes \c parent to QWidget's constructor.
     */
    explicit FlightProgressWidget(QWidget* parent = nullptr);
    
    const Core::Pilot* flight() const { return m_flight.data(); }
    void setFlight(const Core::Pilot* flight);
    
private slots:
    void progressChanged();
    
private:
    QPointer<const Core::Pilot> m_flight;
    
}; /** @} */

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_FLIGHTPROGRESSWIDGET_H
