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


#include "network/HttpHandler.h"

#include "vatsimdata/VatsimDataHandler.h"

#include "MetarsHandler.h"
#include "defines.h"

MetarsHandler::MetarsHandler(HttpHandler* _hh, QObject* _parent) :
		QObject(_parent),
		__httpHandler(_hh) {
	
	connect(__httpHandler, SIGNAL(finished(QString)), this, SLOT(gotMetar(QString)));
}

void
MetarsHandler::fetchMetar(const QString& _icao) {
	__httpHandler->fetchData(VatsimDataHandler::GetSingleton().getMetarUrl() + "?id=" + _icao.toLower());
	__requests.enqueue(_icao.simplified());
}

void
MetarsHandler::updateAllMetars() {
	for (Metar& m: __metars)
		fetchMetar(m.icao);
}

void
MetarsHandler::clear() {
	__metars.clear();
	emit newMetarsAvailable();
}

const Metar *
MetarsHandler::find(const QString& _key) const {
	for (const Metar& m: __metars)
		if (m.icao == _key)
			return &m;
	
	return NULL;
}

void
MetarsHandler::gotMetar(const QString& _metar) {
	QString metar = _metar.simplified();
	if (metar.isEmpty())
		return;
	
	if (metar.contains(METAR_NO_AVAIL)) {
		emit noMetar();
		return;
	}
	
	QString oneMetar;
	for (QString& word: metar.split(' ')) {
		if (__matches(word)) {
			if (!oneMetar.isEmpty())
				__addMetar(oneMetar);
			oneMetar.clear();
		}
		oneMetar.append(word + " ");
	}
	
	if (!oneMetar.isEmpty())
		__addMetar(oneMetar);
	
	__requests.dequeue();

	emit newMetarsAvailable();
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
