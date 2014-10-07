/*
 * newversiondialog.h
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

#ifndef NEWVERSIONDIALOG_H
#define NEWVERSIONDIALOG_H

#include <QDialog>
#include "ui/ui_newversiondialog.h"

/**
 * The NewVersionDialog is used to warn user about the new Vatsinator
 * version available to download.
 */
class NewVersionDialog : public QDialog, private Ui::NewVersionDialog {
  
  Q_OBJECT

public:
  explicit NewVersionDialog(QWidget* parent = nullptr);

};

#endif // NEWVERSIONDIALOG_H
