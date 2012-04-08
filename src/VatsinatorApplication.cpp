/*
    VatsinatorApplication.cpp
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

#include <cstdlib>

#include <QtGui>
#include "../include/VatsinatorApplication.h"

#include "../include/AirportsDatabase.h"
#include "../include/FirsDatabase.h"
#include "../include/HttpHandler.h"
#include "../include/UserInterface.h"
#include "../include/VatsimDataHandler.h"

#include "../include/defines.h"

VatsinatorApplication::VatsinatorApplication(int& _argc, char** _argv) :
		QApplication(_argc, _argv),
		__airportsData(new AirportsDatabase),
		__firsData(new FirsDatabase),
		__vatsimData(new VatsimDataHandler),
		__userInterface(new UserInterface) {
	
	connect(&__timer, SIGNAL(timeout()), this, SLOT(refreshData()));
	
	__airportsData->init();
	__firsData->init();
	
	__userInterface->show();
	__httpHandler = new HttpHandler(__userInterface->getProgressBar());
	
	connect(__httpHandler, SIGNAL(finished(QString)), this, SLOT(__statusFileUpdated(QString)));
	
	__fetchStatusFile();
	
	__timer.start(REFRESH_RATE);
}

VatsinatorApplication::~VatsinatorApplication() {
	qDebug() << "Deleting VatsinatorApplication...";
	
	if (__httpHandler)
		delete __httpHandler;
	
	if (__userInterface)
		delete __userInterface;
	
	if (__vatsimData)
		delete __vatsimData;
	
	if (__airportsData)
		delete __airportsData;
	
	qDebug() << "VatsinatorApplication deleted.";
}

void
VatsinatorApplication::quit() {
	exit(0);
}

void
VatsinatorApplication::alert(const char* _msg) {
	QMessageBox msgBox;
	msgBox.setText(tr(_msg));
	msgBox.exec();
}

void
VatsinatorApplication::refreshData() {
	__userInterface->statusBarUpdate("Fetching data...");
	__httpHandler->fetchData(__vatsimData->getDataUrl());
	__timer.stop();
	__timer.start(REFRESH_RATE);
}

void
VatsinatorApplication::__statusFileUpdated(QString _data) {
	if (_data.isEmpty()) {
		QMessageBox decision;
		decision.setText(tr("Vatsinator was unable to fetch Vatsim's status file."));
		decision.setInformativeText(tr("What do you want to do with that?"));
		QPushButton* againButton = decision.addButton(tr("Try again"), QMessageBox::ActionRole);
		decision.addButton(tr("Cancel"), QMessageBox::RejectRole);
		decision.setIcon(QMessageBox::Warning);
		
		decision.exec();
		
		if (decision.clickedButton() == againButton) {
			__fetchStatusFile();
			return;
		} else {
			__userInterface->statusBarUpdate();
			return;
		}
	}
	
	__vatsimData->parseStatusFile(_data);
	
	disconnect(__httpHandler, SIGNAL(finished(QString)), this, SLOT(__statusFileUpdated(QString)));
	connect(__httpHandler, SIGNAL(finished(QString)), this, SLOT(__dataFileUpdated(QString)));
	
	__userInterface->statusBarUpdate();
	refreshData();
}

void
VatsinatorApplication::__dataFileUpdated(QString _data) {
	__userInterface->statusBarUpdate();
	if (_data.isEmpty()) {
		QMessageBox decision;
		decision.setText(tr("Vatsinator was unable to fetch Vatsim's data file."));
		decision.setInformativeText(tr("What do you want to do with that?"));
		QPushButton* againButton = decision.addButton(tr("Try again"), QMessageBox::ActionRole);
		decision.addButton(tr("Keep current data"), QMessageBox::RejectRole);
		decision.setIcon(QMessageBox::Warning);
		
		decision.exec();
		
		if (decision.clickedButton() == againButton) {
			refreshData();
			return;
		} else
			return;
	}
	
	QString temp;
	if (__userInterface->getGLContext()->getTrackedPilot())
		temp = __userInterface->getGLContext()->getTrackedPilot()->callsign;
	
	__vatsimData->parseDataFile(_data);
	__userInterface->getClientsBox()->setText("Clients: " + QString::number(
			__vatsimData->getPilots().size() + __vatsimData->getATCs().size()
		));
	
	if (__userInterface->getGLContext()->getTrackedPilot())
		__userInterface->getGLContext()->getTrackedPilot() = __vatsimData->findPilot(temp);
	__userInterface->getGLContext()->paintGL();
}

void
VatsinatorApplication::__fetchStatusFile() {
	__userInterface->statusBarUpdate("Fetching data...");
	__httpHandler->fetchData(VATSIM_STATUS_URL);
}