/*
 * basewindow.h
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

#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QWidget>

/**
 * BaseWindow is a base class for all Vatsinator's subwindows.
 * It ensures that all windows open on the correct screen,
 * in the correct place. Use it to gain consistent look'n'feel.
 * 
 * \deprecated
 * \todo Remove.
 */
class BaseWindow : public QWidget {
  Q_OBJECT
  
public:
  BaseWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);

protected:
  void showEvent(QShowEvent* event);
  
private:
  bool __windowSetup; /* Indicates whether the window paremeters
                         were set up in showEvent() */

};

#endif // BASEWINDOW_H
