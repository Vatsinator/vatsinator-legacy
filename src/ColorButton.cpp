/*
    ColorButton.cpp
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

#include "../include/ColorButton.h"

ColorButton::ColorButton(QWidget* _parent) :
		QPushButton("", _parent),
		__current(NULL) {
	connect(this,	SIGNAL(clicked()),
		this,	SLOT(__handleClicked()));
}

void
ColorButton::setColor(QColor* _color) {
	__current = _color;
	updateColor();
}

void
ColorButton::updateColor() {
	setStyleSheet("background: rgb(" +
		QString::number(__current->red()) + "," +
		QString::number(__current->green()) + "," +
		QString::number(__current->blue()) + ");");
}

void
ColorButton::__handleClicked() {
	emit clicked(__current);
}

