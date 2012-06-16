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

#include "ui/windows/airportdetailswindow.h"

#include "vatsimdata/airport.h"

#include "showairportbutton.h"
#include "defines.h"

ShowAirportButton::ShowAirportButton(QWidget* _parent) :
		QPushButton("", _parent),
		__current(NULL) {
	connect(this, SIGNAL(clicked()), this, SLOT(__handleClicked()));
	// we assume that autoconnect is on
	connect(this,						SIGNAL(clicked(const Airport*)),
		AirportDetailsWindow::GetSingletonPtr(),	SLOT(show(const Airport*)));
}

ShowAirportButton::ShowAirportButton(const Airport* _ap, const QString& _text, bool _autoconnect, QWidget* _parent) :
		QPushButton(_text, _parent),
		__current(_ap) {
	connect(this, SIGNAL(clicked()), this, SLOT(__handleClicked()));
	if (_autoconnect)
		connect(this,						SIGNAL(clicked(const Airport*)),
			AirportDetailsWindow::GetSingletonPtr(),	SLOT(show(const Airport*)));
}

void
ShowAirportButton::setAirportPointer(const Airport* _ap) {
	__current = _ap;
	setEnabled(_ap != NULL);
	setFlat(_ap == NULL);
}

void
ShowAirportButton::__handleClicked() {
	if (__current)
		emit clicked(__current);
}
