/*
    settingswindow.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QVector>

#include "ui/ui_settingswindow.h"

class AbstractSettingsModule;
class QWidget;
class QMacToolBar;
class QSignalMapper;

/**
 * Settings window lets the user customize Vatsinator.
 *
 * On Windows and Linux page list is displayed in a list view, on the
 * left side. On OSX QMacToolBar is used to get the most native look.
 * Moreover, OSX does not have "Accept/Save/Restore Defaults" buttons.
 */
class SettingsWindow : public QWidget, private Ui::SettingsWindow {
    Q_OBJECT
    
signals:
    /**
     * User clicks _Restore Defaults_ button.
     *
     * This signal has no use on OSX.
     */
    void restoreDefaults();
    
    /**
     * Emited when user applies the settings.
     *
     * This signal is emitted on Windows and Linux when user clicks
     * _Apply_/_OK_ button and on OSX when user closes the window.
     */
    void settingsApplied();
    
    /**
     * Emitted when user changes anything in the settings window.
     *
     * This signal makes use only on OSX, where settings are applied
     * immediately. They are updated in SettingsManager, but are
     * not saved to file until settingsApplied() is emitted.
     */
    void settingsChanged();
    
public:
    SettingsWindow(QWidget* parent = nullptr);
    
protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    
private:
    void __addPage(const QString& element, const QString& icon, QWidget* page);
    
private slots:
    void __handleButton(QAbstractButton* button);
    void __resizeToMinimum();
    
private:
    QMacToolBar* __macToolBar;
    QSignalMapper* __macMapper;
    
};

#endif // SETTINGSWINDOW_H
