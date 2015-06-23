/*
 * widgetsettingsmodule.h
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#ifndef WIDGETSETTINGSMODULE_H
#define WIDGETSETTINGSMODULE_H

#include <QWidget>

#include "storage/abstractsettingsmodule.h"

/**
 * The WidgetSettingsModule class is an utility class for QWidget-based
 * settings pages.
 */
class WidgetSettingsModule : public QWidget, public AbstractSettingsModule {
  Q_OBJECT
  
public:
  WidgetSettingsModule(QWidget* parent = nullptr);
  
  /**
   * \copydoc AbstractSettingsModule::update()
   * Captures all QWidget-based classes and tries to resolve their
   * key in settings using the \ref settingsKey() routine. If the key
   * is not empty, updates the given value.
   */
  void update() const override;
  
protected:
  /**
   * Resolves the given _widget_'s key.
   * The default implementation firstly queries `vatsinatorSettingsKey`
   * property (it must be string), if it is empty then gets the _widget_'s
   * name. An empty key is invalid.
   */
  virtual QString settingsKey(const QWidget *widget) const;
  
  /**
   * \copydoc AbstractSettingsModule::restore().
   */
  void restore(QSettings &settings, const QVariantHash &defaults) override;
  
  /**
   * \copydoc AbstractSettingsModule::save().
   */
  void save(QSettings &settings) override;

};

#endif // WIDGETSETTINGSMODULE_H
