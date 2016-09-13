/*
 * settingswindow.h
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

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QScopedPointer>

namespace Vatsinator { namespace Core { class Option; }}
namespace Ui { class SettingsWindow; }
class QAbstractButton;

class SettingsWindow : public QWidget {
    Q_OBJECT
    
public:
    explicit SettingsWindow(QWidget* parent = nullptr);
    virtual ~SettingsWindow();
    
protected:
    void showEvent(QShowEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    void fillLanguages();
    
private slots:
    void handleButton(QAbstractButton* button);
    
private:
    QScopedPointer<Ui::SettingsWindow> ui;
    QList<Vatsinator::Core::Option*> m_options;
};

#endif // SETTINGSWINDOW_H
