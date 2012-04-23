/*
    Fir.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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

#include <QtGui>

#include "ui/mapwidget/MapWidget.h"

#include "Fir.h"

#include "defines.h"

Fir::Fir() {
}

Fir::~Fir() {
	MapWidget::deleteImage(icaoTip);
}

void
Fir::addStaff(const Controller* _c) {
	__staff.push_back(_c);
}

void
Fir::addUirStaff(const Controller* _c) {
	__uirStaff.push_back(_c);
}

void
Fir::generateTip() {
	QString icao(header.icao);
	if (header.oceanic) {
		icao = icao.left(4) + " Oceanic";
	}
	icao = icao.simplified();
	
	if (header.textPosition.x == 0.0 && header.textPosition.y == 0.0) {
		icaoTip = 0;
		return;
	}
	
	QImage temp(MapWidget::GetSingleton().getFirToolTipBackground());
	QPainter painter(&temp);
	painter.setRenderHint(QPainter::TextAntialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);
	painter.setFont(MapWidget::GetSingleton().getFirFont());
	painter.setPen(QColor(FIRS_LABELS_FONT_COLOR));
	QRect rectangle(0, 0, 64, 24);
	painter.drawText(rectangle, Qt::AlignCenter | Qt::TextWordWrap, icao);
	icaoTip = MapWidget::loadImage(temp);
}