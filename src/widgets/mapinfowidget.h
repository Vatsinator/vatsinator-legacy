/*
 * mapinfowidget.h
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

#ifndef WIDGETS_MAPINFOWIDGET_H
#define WIDGETS_MAPINFOWIDGET_H

#include "widgetsexport.h"
#include <QDateTime>
#include <QScopedPointer>
#include <QWidget>

namespace Ui { class MapInfoWidget; }

namespace Vatsinator { namespace Widgets {

/**
 * \ingroup Widgets
 * @{
 */
class __VtrWidgetsApi__ MapInfoWidget : public QWidget {
    Q_OBJECT

    Q_PROPERTY(QDateTime updated READ updated WRITE setUpdated)
    
public:
    explicit MapInfoWidget(QWidget* parent = nullptr);
    virtual ~MapInfoWidget();
    
    const QDateTime& updated() const { return m_updated; }
    void setUpdated(const QDateTime& updated);
    int clients() const { return m_clients; }
    void setClients(int clients);
    int pilots() const { return m_pilots; }
    void setPilots(int pilots);
    int atcs() const { return m_atcs; }
    void setAtcs(int atcs);
    int ovservers() const { return m_observers; }
    void setObservers(int observers);
    
private:
    void updateStatusLabel();
    
    QScopedPointer<Ui::MapInfoWidget> ui;
    QDateTime m_updated;
    int m_clients = 0;
    int m_pilots = 0;
    int m_atcs = 0;
    int m_observers = 0;

}; /** @} */

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_MAPINFOWIDGET_H
