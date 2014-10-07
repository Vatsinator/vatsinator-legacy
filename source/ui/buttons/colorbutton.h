/*
    colorbutton.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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


#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

/**
 * The ColorButton provides easy way to show and let pick any color.
 */
class ColorButton : public QPushButton {
  Q_OBJECT

  /**
   * This property holds the picked color.
   */
  Q_PROPERTY(QColor color READ color WRITE setColor)
  
public:
  /**
   * The default constrcutor passes _parent_ to QPushButton.
   */
  ColorButton(QWidget* parent = nullptr);
  
  void setColor(const QColor& color);
  
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

  inline const QColor& color() { return __current; }
  
protected:
  void paintEvent(QPaintEvent* event) override;
  
private:
  void __initStyleOptionButton(QStyleOptionButton* option) const;

private slots:
  /**
   * Opens the color dialog in order to let the user pick the color
   */
  void __pickColor();

private:
  QColor  __current;
};

#endif // COLORBUTTON_H
