/*
    showairportbutton.cpp
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

#include "showairportbutton.h"
#include "defines.h"

ShowAirportButton::ShowAirportButton(const AirportObject* _ap, QWidget* _parent) :
		QPushButton("Show", _parent),
		__current(_ap) {
	connect(this, SIGNAL(clicked()), this, SLOT(__handleClicked()));
}

void
ShowAirportButton::__handleClicked() {
	emit clicked(__current);
}
