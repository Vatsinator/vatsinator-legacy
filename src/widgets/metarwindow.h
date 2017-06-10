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

#include "core/servertracker.h"
#include "widgets/vtrwidgets_export.h"
#include <QtWidgets/QWidget>
#include <QtCore/QPointer>

namespace Ui { class MetarWindow; }

namespace Vatsinator { namespace Widgets {

/**
 * \ingroup Widgets
 * @{
 *
 * The MetarWindow displays list of all METAR weather reports.
 */
class VTRWIDGETS_EXPORT MetarWindow : public QWidget {
    Q_OBJECT

public:
    explicit MetarWindow(Vatsinator::Core::ServerTracker* server, QWidget* parent = nullptr);

    /**
     * Destroys this \c MetarWindow.
     */
    virtual ~MetarWindow();

    using QWidget::show;

public slots:
    void show(const QString& icaoSearch);

protected:
    /**
     * \copydoc QWidget::showEvent()
     */
    void showEvent(QShowEvent* event) override;

private slots:
    void queryChanged();

private:
    QScopedPointer<Ui::MetarWindow> ui;

};

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_METARWINDOW_H
