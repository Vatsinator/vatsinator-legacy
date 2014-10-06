/*
    colorbutton.cpp
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

#include <QtWidgets>
#include <qdrawutil.h>
#include <QApplication>

#include "colorbutton.h"

ColorButton::ColorButton(QWidget* parent) :
    QPushButton(parent) {
  connect(this, SIGNAL(clicked()),
          this, SLOT(__pickColor()));
}

void
ColorButton::setColor(const QColor& color) {
  if (__current != color) {
    __current = color;
    update();
  }
}

QSize
ColorButton::sizeHint() const {
  QStyleOptionButton optBtn;
  __initStyleOptionButton(&optBtn);
  return style()->sizeFromContents(QStyle::CT_PushButton, &optBtn, QSize(40, 15), this).
      expandedTo(QApplication::globalStrut());
}

QSize
ColorButton::minimumSizeHint() const {
  QStyleOptionButton optBtn;
  __initStyleOptionButton(&optBtn);
  return style()->sizeFromContents(QStyle::CT_PushButton, &optBtn, QSize(3, 3), this).
      expandedTo(QApplication::globalStrut());
}

void
ColorButton::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  
  // init painter and style
  QPainter painter(this);
  QStyle* style = QWidget::style();
  
  // first we need to draw bevel
  QStyleOptionButton optBtn;
  __initStyleOptionButton(&optBtn);
  style->drawControl(QStyle::CE_PushButtonBevel, &optBtn, &painter, this);
  
  // get coords of rectangle that we will fill with desired color
  QRect labelRect = style->subElementRect(QStyle::SE_PushButtonContents, &optBtn, this);
  int shift = style->pixelMetric(QStyle::PM_ButtonMargin, &optBtn, this) / 2;
  labelRect.adjust(shift, shift, -shift, -shift);
  
  // put coords into variables
  int x, y, w, h;
  labelRect.getRect(&x, &y, &w, &h);
  if (isChecked() || isDown()) {
    x += style->pixelMetric( QStyle::PM_ButtonShiftHorizontal, &optBtn, this );
    y += style->pixelMetric( QStyle::PM_ButtonShiftVertical, &optBtn, this );
  }
  
  // fill with current color
  QBrush brush(__current);
  
  // fill rectangle(x, y, w, h) with brush
  qDrawShadePanel(&painter, x, y, w, h, palette(), true, 1, &brush);
  
  if (hasFocus()) {
    QRect focusRect = style->subElementRect(QStyle::SE_PushButtonFocusRect, &optBtn, this);
    QStyleOptionFocusRect focusOpt;
    focusOpt.init(this);
    focusOpt.rect = focusRect;
    focusOpt.backgroundColor = palette().background().color();
    style->drawPrimitive(QStyle::PE_FrameFocusRect, &focusOpt, &painter, this);
  }
}

void
ColorButton::__initStyleOptionButton(QStyleOptionButton* option) const {
  option->initFrom(this);
  option->state |= isDown() ? QStyle::State_Sunken : QStyle::State_Raised;
  option->features = QStyleOptionButton::None;
  option->text.clear();
  option->icon = QIcon();
}

void
ColorButton::__pickColor() {
  QColorDialog* dialog = new QColorDialog(__current);
  dialog->setWindowTitle(tr("Select color"));

  if (dialog->exec() == QDialog::Accepted)
    __current = dialog->currentColor();

  delete dialog;

  update();
}

