/*
 * widgetsettingsmodule.cpp
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

#include <QtWidgets>

#include "ui/buttons/colorbutton.h"

#include "widgetsettingsmodule.h"

WidgetSettingsModule::WidgetSettingsModule(QWidget* parent) : QWidget(parent) {}

void
WidgetSettingsModule::update() const
{
    auto widgets = this->findChildren<QWidget*>();
    std::for_each(widgets.begin(), widgets.end(), [this](const QWidget * w) {
        QString key = settingsKey(w);
        
        if (key.isEmpty())
            return;
            
        if (const QCheckBox* cb = qobject_cast<const QCheckBox*>(w))
            setValue(key, cb->isChecked());
        else if (const QComboBox* cb = qobject_cast<const QComboBox*>(w))
            setValue(key, cb->currentIndex());
        else if (const QSlider* slider = qobject_cast<const QSlider*>(w))
            setValue(key, slider->value());
        else if (const QSpinBox* sb = qobject_cast<const QSpinBox*>(w))
            setValue(key, sb->value());
        else if (const ColorButton* button = qobject_cast<const ColorButton*>(w))
            setValue(key, button->color());
        else
            qDebug() << w->metaObject()->className() << "not supported by WidgetSettingsModule";
    });
}

QString
WidgetSettingsModule::settingsKey(const QWidget* widget) const
{
    QVariant prop = widget->property("vatsinatorSettingsKey");
    
    if (!prop.isValid() || prop.isNull() || !prop.canConvert<QString>() || prop.toString().isEmpty())
        return QString();
    else
        return prop.toString();
}

void
WidgetSettingsModule::restore(QSettings& settings, const QVariantHash& defaults)
{
    auto widgets = this->findChildren<QWidget*>();
    std::for_each(widgets.begin(), widgets.end(), [this, &settings, &defaults](QWidget * w) {
        QString key = settingsKey(w);
        
        if (key.isEmpty())
            return;
            
        QVariant s = settings.value(key, defaults[moduleId() % "." % key]);
        
        if (QCheckBox* cb = qobject_cast<QCheckBox*>(w))
            cb->setChecked(s.toBool());
        else if (QComboBox* cb = qobject_cast<QComboBox*>(w))
            cb->setCurrentIndex(s.toInt());
        else if (QSlider* slider = qobject_cast<QSlider*>(w))
            slider->setValue(s.toInt());
        else if (QSpinBox* sb = qobject_cast<QSpinBox*>(w))
            sb->setValue(s.toInt());
        else if (ColorButton* button = qobject_cast<ColorButton*>(w))
            button->setColor(s.value<QColor>());
        else
            qDebug("%s not supported by WidgetSettingsModule", w->metaObject()->className());
    });
}

void
WidgetSettingsModule::save(QSettings& settings)
{
    auto widgets = this->findChildren<QWidget*>();
    std::for_each(widgets.begin(), widgets.end(), [this, &settings](QWidget * w) {
        QString key = settingsKey(w);
        
        if (key.isEmpty())
            return;
            
        if (QCheckBox* cb = qobject_cast<QCheckBox*>(w))
            settings.setValue(key, cb->isChecked());
        else if (QComboBox* cb = qobject_cast<QComboBox*>(w))
            settings.setValue(key, cb->currentIndex());
        else if (QSlider* slider = qobject_cast<QSlider*>(w))
            settings.setValue(key, slider->value());
        else if (QSpinBox* sb = qobject_cast<QSpinBox*>(w))
            settings.setValue(key, sb->value());
        else if (ColorButton* button = qobject_cast<ColorButton*>(w))
            settings.setValue(key, button->color());
        else
            qDebug() << w->metaObject()->className() << "not supported by WidgetSettingsModule";
    });
}
