/*
 * idldrawer.h
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

#ifndef IDLDRAWER_H
#define IDLDRAWER_H

#include "core/lonlat.h"
#include "gui/mapaddon.h"
#include <QObject>
#include <QList>

/**
 * \defgroup idl-drawer Draws the Internation Date Line.
 * @{
 */
class IdlDrawer : public QObject, public Vatsinator::Gui::MapAddon {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.vtrgui.MapAddon" FILE "idldrawer.json")
    Q_INTERFACES(Vatsinator::Gui::MapAddon)

public:
    explicit IdlDrawer(QObject *parent = nullptr);

    void drawBeforeItems(Vatsinator::Gui::WorldPainter* painter) override;
    void drawAfterItems(Vatsinator::Gui::WorldPainter* painter) override;

private:
    QList<Vatsinator::Core::LonLat> m_idl;

}; /** @} */

#endif // IDLDRAWER_H
