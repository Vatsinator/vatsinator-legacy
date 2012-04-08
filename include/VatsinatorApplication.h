/*
    VatsinatorApplication.h
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


#ifndef VATSINATORAPPLICATION_H
#define VATSINATORAPPLICATION_H

#include <QApplication>
#include <QTimer>

#include "Singleton.h"

class AirportsDatabase;
class FirsDatabase;
class UserInterface;
class VatsimDataHandler;
class HttpHandler;

class VatsinatorApplication :
		public QApplication,
		public Singleton< VatsinatorApplication > {
	
	/*
	 * This class handles the whole Vatsinator application and has
	 * only one instance.
	 */
	
	Q_OBJECT
	
public:
	/**
	 * Constructor gives argc & argv to the QApplication.
	 */
	VatsinatorApplication(int&, char**);
	
	virtual ~VatsinatorApplication();
	
	void quit();
	
	void alert(const char*);
	
	VatsimDataHandler& getData() { return *__vatsimData; }

public slots:
	void refreshData();
	
private slots:
	void __statusFileUpdated(QString);
	void __dataFileUpdated(QString);
	
	
private:
	void		__fetchStatusFile();
	
	HttpHandler *		__httpHandler;
	
	AirportsDatabase *	__airportsData;
	
	FirsDatabase * 		__firsData;
	
	VatsimDataHandler *	__vatsimData;
	
	UserInterface *		__userInterface;
	
	QTimer 				__timer;
	
};

#endif // VATSINATORAPPLICATION_H

struct Metar;
