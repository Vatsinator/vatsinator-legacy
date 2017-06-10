/*
 * atcdetailswindow.h
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

#ifndef WIDGETS_ATCDETAILSWINDOW_H
#define WIDGETS_ATCDETAILSWINDOW_H

#include "core/atc.h"
#include "widgets/vtrwidgets_export.h"
#include <QtCore/QPointer>
#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>

namespace Ui { class AtcDetailsWindow; }

namespace Vatsinator { namespace Widgets {

/**
 * \ingroup Widgets
 * @{
 * 
 * A window for displaying ATC's details.
 */
class VTRWIDGETS_EXPORT AtcDetailsWindow : public QWidget {
    Q_OBJECT
    
    /**
     * Keeps the ATC instance.
     */
    Q_PROPERTY(const Vatsinator::Core::Atc* atc READ atc WRITE setAtc)

public:
    /**
     * Creates a new AtcDetailsWindow. Passes \c parent to the
     * QWidget's constructor.
     */
    explicit AtcDetailsWindow(QWidget* parent = nullptr);
    
    /**
     * Destroys the AtcDetailsWindow.
     */
    virtual ~AtcDetailsWindow();
    
    const Core::Atc* atc() const { return m_atc.data(); }
    void setAtc(const Core::Atc* atc);
    
protected:
    /**
     * \copydoc QWidget::showEvent()
     */
    void showEvent(QShowEvent* event) override;
    
    /**
     * \copydoc QWidget::changeEvent()
     */
    void changeEvent(QEvent* event) override;
    
private:
    void atcUpdated();
    
private slots:
    void updateCallsign(QString callsign);
    void updateDescription(QString description);
    void updateRealName(QString realName);
    void updateFrequency(QString frequency);
    void updateRating(Core::Atc::Rating rating);
    void updateAirport(Core::Airport* airport);
    void updateServer(QString server);
    void updateOnlineFrom(QDateTime onlineFrom);
    void updateAtis(QString atis);
    
private:
    QScopedPointer<Ui::AtcDetailsWindow> ui;
    QPointer<const Core::Atc> m_atc;

}; /** @} */

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_ATCDETAILSWINDOW_H
