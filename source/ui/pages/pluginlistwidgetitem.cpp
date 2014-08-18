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

PluginListWidgetItem::PluginListWidgetItem(const QString& _text,
                                           QListWidget* _parent,
                                           int _type):
    QListWidgetItem(_text, _parent, _type),
    __simplified(_text.toLower()) {
  __simplified.replace(' ', QString());
}

QVariant
PluginListWidgetItem::data(int _role) const {
  if (_role == Qt::UserRole)
    return __simplified;
  else
    return QListWidgetItem::data(_role);
}

void
PluginListWidgetItem::setData(int _role, const QVariant& _data) {
  if (_role == Qt::UserRole)
    __simplified = _data.toString();
  else
    QListWidgetItem::setData(_role, _data);
}