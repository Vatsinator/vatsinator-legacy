/*
 * vatsimmessagedialog.h
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

#ifndef VATSIMMESSAGEDIALOG_H
#define VATSIMMESSAGEDIALOG_H

#include <QDialog>

#include "ui/ui_vatsimmessagedialog.h"

/**
 * The VatsimMessageDialog shows Vatsim message that comes after "msg0="
 * in status.txt.
 */
class VatsimMessageDialog : public QDialog, public Ui::VatsimMessageDialog {
  Q_OBJECT

public:
  explicit VatsimMessageDialog(const QString& message, QWidget* parent = nullptr);
    
protected:
  void hideEvent(QHideEvent* event) override;
  
private:
  QString __message;
  
  
};

#endif // VATSIMMESSAGEDIALOG_H
