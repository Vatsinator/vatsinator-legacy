/*
 * letsendstatsdialog.h
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

#ifndef LETSENDSTATSDIALOG_H
#define LETSENDSTATSDIALOG_H

#include <QDialog>
#include "ui/ui_letsendstatsdialog.h"

/**
 * The LetSendStatsDialog provides dialog that lets user decide whether
 * he wants to send anonymous statistics or not.
 */
class LetSendStatsDialog : public QDialog, private Ui::LetSendStatsDialog {
  Q_OBJECT

public:
  explicit LetSendStatsDialog(QWidget* parent = nullptr);

};

#endif // LETSENDSTATSDIALOG_H
