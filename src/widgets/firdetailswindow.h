/*
 * firdetailswindow.h
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

#ifndef WIDGETS_FIRDETAILSWINDOW_H
#define WIDGETS_FIRDETAILSWINDOW_H

#include "widgetsexport.h"
#include "core/firobject.h"
#include <QWidget>
#include <QScopedPointer>

namespace Ui { class FirDetailsWindow; }

namespace Vatsinator { namespace Widgets {

/**
 * \ingroup Widgets
 * @{
 * 
 * A window for displaying FIR's details.
 */
class __VtrWidgetsApi__ FirDetailsWindow : public QWidget {
    Q_OBJECT
    
    Q_PROPERTY(const Vatsinator::Core::FirObject* fir READ fir WRITE setFir)
    
signals:
    /**
     * Emitted when a client's details are requested.
     * \param callsign The requested client's callsign.
     */
    void clientDetailsRequested(const QString& callsign);
    
public:
    explicit FirDetailsWindow(QWidget* parent = nullptr);
    
    virtual ~FirDetailsWindow();
    
    const Core::FirObject* fir() const { return m_fir; }
    void setFir(const Core::FirObject* fir);
    
protected:
    /**
     * \copydoc QWidget::showEvent()
     */
    void showEvent(QShowEvent* event) override;
    
private slots:
    void showDetails(const QModelIndex& index);
    void updateNotams();
    void updateBookings();

private:
    QScopedPointer<Ui::FirDetailsWindow> ui;
    const Core::FirObject* m_fir = nullptr;
    
}; /** @} */

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_FIRDETAILSWINDOW_H
