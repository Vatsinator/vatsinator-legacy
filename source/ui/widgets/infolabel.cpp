/*
 * infolabel.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#include <QtWidgets>

#include "ui/vatsinatorstyle.h"
#include "vatsinatorapplication.h"

#include "infolabel.h"

InfoLabel::InfoLabel(QWidget* parent, Qt::WindowFlags flags) :
    QWidget(parent, flags),
    __descriptionFont(qobject_cast<VatsinatorStyle*>(vApp()->style())->smallFont()) {}

void
InfoLabel::setDescription(const QString& description) {
  __description = description;
  updateGeometry();
}

void
InfoLabel::setValue(const QString& value) {
  __value = value;
  updateGeometry();
}

void
InfoLabel::setDescriptionFont(const QFont& font) {
  __descriptionFont = font;
  updateGeometry();
}

QSize
InfoLabel::sizeHint() const {
  QFontMetrics fm1 = QFontMetrics(__descriptionFont);
  QFontMetrics fm2 = fontMetrics();
  
  return QSize(
    qMax(fm1.width(description()), fm2.width(value())),
    fm1.height() + fm2.height()
  );
}

QSize
InfoLabel::minimumSizeHint() const {
  QFontMetrics fm1 = QFontMetrics(__descriptionFont);
  QFontMetrics fm2 = fontMetrics();
  
  return QSize(
    qMax(fm1.width(description()), fm2.width(value())),
    fm1.height() + fm2.height()
  );
}

void
InfoLabel::paintEvent(QPaintEvent* event) {
  QPainter p(this);
  
  QFontMetrics descFm = QFontMetrics(__descriptionFont);
  QFontMetrics valueFm = fontMetrics();
  
  QRect descRect(event->rect().topLeft(), QSize(event->rect().width(), descFm.height()));
  p.setFont(__descriptionFont);
  p.drawText(descRect, Qt::AlignCenter, description());
  
  QRect valueRect(event->rect().topLeft(), QSize(event->rect().width(), valueFm.height()));
  valueRect.moveTop(descRect.bottom());
  p.setFont(font());
  p.drawText(valueRect, Qt::AlignCenter, value());
}
