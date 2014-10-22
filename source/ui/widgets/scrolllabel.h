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

/**
 * The ScrollLabel is a widget that looks like QLabel but it scrolls text
 * inside if it is too long to fit in it.
 */
class ScrollLabel : public QWidget {  
  Q_OBJECT
  
  /**
   * This property holds the text to be rendered.
   */
  Q_PROPERTY(QString text READ text WRITE setText)
  
public:
  /**
   * The default constructor passes _parent_ to QWidget.
   */
  explicit ScrollLabel(QWidget* parent = nullptr);
  
  inline const QString& text() const { return __text; }
  
public slots:
  void setText(const QString& text);
  
protected:
  void paintEvent(QPaintEvent* event);
  
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
