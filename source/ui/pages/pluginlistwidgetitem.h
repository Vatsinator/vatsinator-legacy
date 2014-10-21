/*
 * pluginlistwidgetitem.h
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

#ifndef PLUGINLISTWIDGETITEM_H
#define PLUGINLISTWIDGETITEM_H

#include <QListWidgetItem>

/**
 * The PluginListWidgetItem is a convenience class to represent a single
 * plugin on the plugin list. It provides simplified name under
 * Qt::UserRole.
 */
class PluginListWidgetItem : public QListWidgetItem {

public:
  explicit PluginListWidgetItem(const QString& text, QListWidget* parent = 0,
                                int type = Type);
  
  QVariant data(int) const override;
  void setData(int, const QVariant&) override;
  
private:
  QString __simplified;
  
};

#endif // PLUGINLISTWIDGETITEM_H
