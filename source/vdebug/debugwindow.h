/*
    debugwindow.h
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#ifndef NO_DEBUG

#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include "vdebug/ui_debugwindow.h"
#include "singleton.h"

class DebugWindow :
    public QWidget,
    public Singleton< DebugWindow >,
    private Ui::DebugWindow {

  Q_OBJECT

public:
  DebugWindow(QWidget* = 0);

public slots:
  void show();

private:
  void __setWindowPosition();
  QString __getGLVersion();

private slots:
  void __updateContents();

};

#endif // DEBUGWINDOW_H

#endif // NO_DEBUG
