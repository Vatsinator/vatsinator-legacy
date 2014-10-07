/*
 * vatsimmessagedialog.cpp
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

#include <QtCore>

#include "vatsimmessagedialog.h"

VatsimMessageDialog::VatsimMessageDialog(const QString& message, QWidget* parent) :
    QDialog(parent),
    __message(message) {
  setupUi(this);
  
  VatsimMessageLabel->setText(message);
}

void
VatsimMessageDialog::hideEvent(QHideEvent* event) {
  Q_UNUSED(event);
  
  if (DoNotShowCheckBox->isChecked()) {
    QString hash = QString::number(qHash(__message));
    QSettings s;
    s.beginGroup("VatsimMessages");
    
    s.setValue(hash, true);
    
    s.endGroup();
  }
}
