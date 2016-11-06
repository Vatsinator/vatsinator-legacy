/*
 * metarwindow.h
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

#ifndef WIDGETS_METARWINDOW_H
#define WIDGETS_METARWINDOW_H

#include "widgetsexport.h"
#include "core/metarmanager.h"
#include <QWidget>
#include <QPointer>

namespace Ui { class MetarWindow; }

namespace Vatsinator { namespace Widgets {

/**
 * \ingroup Widgets
 * @{
 *
 * The MetarWindow displays list of all METAR weather reports.
 */
class __VtrWidgetsApi__ MetarWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * Creates a new MetarWindow instance.
     *
     * \param metars The \ref Vatsinator::Core::MetarManager instance that contains list of METARs.
     * \param parent Passed to QObject's constructor.
     */
    explicit MetarWindow(Vatsinator::Core::MetarManager* metars, QWidget* parent = nullptr);

    /**
     * Destroys this \c MetarWindow.
     */
    virtual ~MetarWindow();

protected:
    /**
     * \copydoc QWidget::showEvent()
     */
    void showEvent(QShowEvent* event) override;

private slots:
    void updateMetars();
    void queryChanged();

private:
    QScopedPointer<Ui::MetarWindow> ui;
    QPointer<Vatsinator::Core::MetarManager> m_metars;
};

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_METARWINDOW_H
