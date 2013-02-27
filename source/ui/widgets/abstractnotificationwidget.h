/*
    abstractnotificationwidget.h
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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


#ifndef ABSTRACTNOTIFICATIONWIDGET_H
#define ABSTRACTNOTIFICATIONWIDGET_H

#include <QWidget>

class AbstractNotificationWidget : public QWidget {
  
  /*
   * Subclass this class in order to display nice notifications above
   * the map in the main window.
   */
  
  Q_OBJECT
  Q_ENUMS(Position);
  
public:
  
  enum Position {
    /*
     * Describes the notification widget position above the main window.
     */
    Top, Bottom
  };
  
  /**
   * We need no parent there, it is set to UserInterface's pointer
   * automatically.
   */
  AbstractNotificationWidget();
  
  virtual QBrush background() const = 0;
  virtual QColor foreground() const = 0;
  virtual Position position() const = 0;
  
  void setText(const QString&);
  void setBold(bool);
  
  /**
   * This widget will fit nicely inside gived rect.
   * @param rect Maximum rect.
   */
  void setBoundingGeometry(const QRect&);
  void setBoundingGeometry(int, int, int, int);
  
  inline const QString &
  getText() const { return __text; }
  
  inline const bool
  isBold() const { return __isbold; }
  
protected:
  void paintEvent(QPaintEvent*);
  
private:
  QString __text;
  bool    __isbold;
  
};

#endif // ABSTRACTNOTIFICATIONWIDGET_H
