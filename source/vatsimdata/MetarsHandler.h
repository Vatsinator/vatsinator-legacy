/*
    MetarsHandler.h
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


#ifndef METARSHANDLER_H
#define METARSHANDLER_H

#include <QObject>
#include <QVector>
#include <QQueue>

#include "Metar.h"

#include "Singleton.h"

class HttpHandler;

class MetarsHandler : public QObject, public Singleton< MetarsHandler > {
	
	/*
	 * This class manages our METARs. It receives any metar requests,
	 * sends signals to MetarsWindow class when new metar is fetched
	 * (or updated) and parses multiple-metars requests.
	 */
	
	Q_OBJECT
	
public:
	/**
	 * This constructor gets HttpHandler class instance pointer to be
	 * used in fetching Metars.
	 */
	MetarsHandler(HttpHandler*, QObject* = 0);
	
	/**
	 * Fetches metar; the string can be 1-4 letters long.
	 */
	void fetchMetar(const QString&);
	
	/**
	 * Looks for the METAR.
	 */
	const Metar * find(const QString&) const;
	
	inline const QVector< Metar > &	getMetars() const { return __metars; }

public slots:
	/**
	 * This is called by MetarsWindow when user clicks "Refresh all".
	 */
	void updateAllMetars();
	
	/**
	 * Clears metars' vector.
	 */
	void clear();
	
signals:
	void newMetarsAvailable();
	void noMetar();
	
private slots:
	void gotMetar(const QString&);
	
private:
	
	/**
	 * Adds/updates given metar.
	 */
	void __addMetar(const QString&);
	
	/**
	 * This formula tells us if the parsed word (in metar's contents) is
	 * a requested airport's ICAO code.
	 */
	bool __matches(const QString&);
	
	QQueue< QString > __requests;
	
	QVector< Metar > __metars;
	
	HttpHandler *	__httpHandler;
	
	
	
	
};

#endif // METARSHANDLER_H
