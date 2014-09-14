/*
    aboutwindow.h
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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


#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include "network/resourcemanager.h"
#include "ui/windows/basewindow.h"
#include "ui/ui_aboutwindow.h"

class AboutWindow :
    public BaseWindow,
    private Ui::AboutWindow {

  Q_OBJECT

public:
  AboutWindow(QWidget* = nullptr);
  
protected:
  void showEvent(QShowEvent*) override;
  
private slots:
  
  /**
   * If the version is outdated, it displays
   * "outdated" warning, otherwise "up-to-date"
   * confirmation gets displayed.
   */
  void __updateVersionStatus(ResourceManager::VersionStatus);

};

#endif // ABOUTWINDOW_H
