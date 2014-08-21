/*
 * datafetcherrordialog.cpp
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

#include "datafetcherrordialog.h"

DataFetchErrorDialog::DataFetchErrorDialog(QWidget* _parent) :
    QMessageBox(_parent),
    __againButton(nullptr),
    __keepButton(nullptr) {
  
  setText(tr("Vatsinator was unable to fetch the data file."));
  setInformativeText(tr("What do you want to do with that?"));
  setIcon(QMessageBox::Question);
  
  __againButton = addButton(tr("Try again"), QMessageBox::ActionRole); 
  __keepButton = addButton(tr("Keep current data"), QMessageBox::RejectRole);
}
