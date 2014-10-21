/*
 * pluginlistwidgetitem.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#include "pluginlistwidgetitem.h"

PluginListWidgetItem::PluginListWidgetItem(const QString& text,
                                           QListWidget* parent,
                                           int type):
    QListWidgetItem(text, parent, type) {
}

QVariant
PluginListWidgetItem::data(int role) const {
  if (role == Qt::UserRole)
    return __simplified;
  else
    return QListWidgetItem::data(role);
}

void
PluginListWidgetItem::setData(int role, const QVariant& data) {
  if (role == Qt::UserRole)
    __simplified = data.toString();
  else
    QListWidgetItem::setData(role, data);
}