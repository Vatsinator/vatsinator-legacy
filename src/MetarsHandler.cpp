/*
    MetarsHandler.cpp
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

#include "../include/MetarsHandler.h"
#include "../include/HttpHandler.h"
#include "../include/VatsimDataHandler.h"

#include "../include/defines.h"

MetarsHandler::MetarsHandler(HttpHandler* _hh, QObject* _parent) :
		QObject(_parent),
		__httpHandler(_hh) {
	
	connect(__httpHandler, SIGNAL(finished(QString)), this, SLOT(gotMetar(QString)));
}

void
MetarsHandler::fetchMetar(const QString& _icao) {
	__httpHandler->fetchData(VatsimDataHandler::GetSingleton().getMetarUrl() + "?id=" + _icao.toLower());
	__requests.push(_icao.simplified());
}

void
MetarsHandler::updateAllMetars() {
	for (Metar& m: __metars)
		fetchMetar(m.icao);
}

void
MetarsHandler::clear() {
	__metars.clear();
	emit newMetarsAvailable(__metars);
}

void
MetarsHandler::gotMetar(QString _metar) {
	_metar = _metar.simplified();
	if (_metar.isEmpty())
		return;
	
	if (_metar.contains(METAR_NO_AVAIL))
		return;
	
	QString oneMetar;
	for (QString& word: _metar.split(' ')) {
		if (__matches(word)) {
			if (!oneMetar.isEmpty())
				__addMetar(oneMetar);
			oneMetar.clear();
		}
		oneMetar.append(word + " ");
	}
	
	if (!oneMetar.isEmpty())
		__addMetar(oneMetar);

	emit newMetarsAvailable(__metars);
}

void
MetarsHandler::__addMetar(const QString& _metar) {
	for (Metar& m: __metars) {
		if (m.icao == _metar.left(4)) {
			m.metar = _metar;
			return;
		}
	}
	
	__metars.push_back(Metar(_metar.left(4), _metar));
}
