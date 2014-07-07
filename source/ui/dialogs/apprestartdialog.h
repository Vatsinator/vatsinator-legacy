/*
 * apprestartdialog.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#ifndef APPRESTARTDIALOG_H
#define APPRESTARTDIALOG_H

#include <QDialog>

#include "ui/ui_apprestartdialog.h"

/**
 * Show dialog that lets the user decide whether he wants to restart
 * the application or not.
 */
class AppRestartDialog :
    public QDialog,
    private Ui::AppRestartDialog {
  
  Q_OBJECT
  
public:
  explicit AppRestartDialog(QWidget* = 0);

};

#endif // APPRESTARTDIALOG_H
