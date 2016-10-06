/*
 * clientlistswindow.h
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

#ifndef WIDGETS_CLIENTLISTWINDOW_H
#define WIDGETS_CLIENTLISTWINDOW_H

#include "widgetsexport.h"
#include "core/servertracker.h"
#include <QModelIndex>
#include <QScopedPointer>
#include <QWidget>

namespace Ui { class ClientListWindow; }

namespace Vatsinator { namespace Widgets {

/**
 * \ingroup Widgets
 * @{
 *
 * A window that shows list of all connected clients.
 *
 * Pilots and ATCs are shown in separate tabs.
 */
class __VtrWidgetsApi__ ClientListWindow : public QWidget {
    Q_OBJECT

signals:
    /**
     * Emitted when a client's details are requested.
     * \param callsign The requested client's callsign.
     */
    void clientDetailsRequested(const QString& callsign);

public:
    /**
     * Creates a new window that shows list of clients connected to the
     * \c server.
     */
    explicit ClientListWindow(const Core::ServerTracker* server, QWidget *parent = nullptr);

    /**
     * Destroys this \c ClientListWindow instance.
     */
    virtual ~ClientListWindow();

protected:
    /**
     * \copydoc QWidget::showEvent()
     */
    void showEvent(QShowEvent* event) override;

    /**
     * \copydoc QWidget::changeEvent()
     */
    void changeEvent(QEvent *e) override;

private slots:
    void showDetails(const QModelIndex& index);

private:
    QScopedPointer<Ui::ClientListWindow> ui;

}; /** @} */

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_CLIENTLISTWINDOW_H
