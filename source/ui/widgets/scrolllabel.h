/*
    scrolllabel.h
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


#ifndef SCROLLLABEL_H
#define SCROLLLABEL_H

#include <QWidget>
#include <QStaticText>
#include <QTimer>

class ScrollLabel : public QWidget {
  
  /*
   * Used as a replacement for QLabel, if the text inside is too long.
   */
  
  Q_OBJECT
  
public:
  explicit ScrollLabel(QWidget* parent = 0);
  
  inline const QString &
  text() const { return __text; }
  
public slots:
  void setText(const QString&);
  
protected:
  void paintEvent(QPaintEvent*);
  
private:
  void __updateText();
  
  QString     __text;
  QStaticText __staticText;
  QSize       __textSize;
  
  int         __textPosition;
  
  QTimer      __timer;
  
private slots:
  void __timerTimeout();
  
};

#endif // SCROLLLABEL_H
