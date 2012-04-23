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

#include <iostream>

#include <QtGui>

#include "db/AirportsDatabase.h"
#include "db/FirsDatabase.h"

#include "network/HttpHandler.h"

#include "ui/SettingsWindow.h"
#include "ui/UserInterface.h"

#include "vatsimdata/VatsimDataHandler.h"

#include "VatsinatorApplication.h"
#include "defines.h"

VatsinatorApplication::VatsinatorApplication(int& _argc, char** _argv) :
		QApplication(_argc, _argv),
		__airportsData(new AirportsDatabase),
		__firsData(new FirsDatabase),
		__vatsimData(new VatsimDataHandler),
		__userInterface(new UserInterface) {
	
#ifndef NO_DEBUG
	std::cout << "AIRPORTS_DB: " << AIRPORTS_DB << std::endl <<
		"FIRS_DB: " << FIRS_DB << std::endl <<
		"VATSINATOR_DAT: " << VATSINATOR_DAT << std::endl;
#endif
	connect(this,			SIGNAL(destroyed()),
		__userInterface,	SLOT(hideAllWindows()));
	
	connect(&__timer, SIGNAL(timeout()), this, SLOT(refreshData()));
	
	__airportsData->init();
	__firsData->init();
	
	__vatsimData->init();
	connect(__vatsimData, SIGNAL(dataCorrupted()), this, SLOT(__showDataAlert()));
	
	__userInterface->show();
	__httpHandler = new HttpHandler(__userInterface->getProgressBar());
	
	connect(__httpHandler, SIGNAL(finished(QString)), this, SLOT(__statusFileUpdated(QString)));
	
	__fetchStatusFile();
	
	__timer.start(SettingsWindow::GetSingleton().getRefreshRate() * 60000);
}

VatsinatorApplication::~VatsinatorApplication() {
	delete __httpHandler;
	delete __userInterface;
	delete __vatsimData;
	delete __airportsData;
	delete __firsData;
	
#ifndef NO_DEBUG
	DumpUnfreed();
#endif
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
	__timer.start(SettingsWindow::GetSingleton().getRefreshRate() * 60000);
}

void
VatsinatorApplication::__statusFileUpdated(const QString& _data) {
	if (_data.isEmpty()) {
		QMessageBox decision;
		decision.setText(tr("Vatsinator was unable to fetch Vatsim's status file."));
		decision.setInformativeText(tr("What do you want to do with that?"));
		QPushButton* againButton = decision.addButton(tr("Try again"), QMessageBox::ActionRole);
		decision.addButton(tr("Cancel"), QMessageBox::RejectRole);
		decision.setIcon(QMessageBox::Warning);
		
		__timer.stop();
		
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
VatsinatorApplication::__dataFileUpdated(const QString& _data) {
	if (_data.isEmpty()) {
		QMessageBox decision;
		decision.setText(tr("Vatsinator was unable to fetch Vatsim's data file."));
		decision.setInformativeText(tr("What do you want to do with that?"));
		QPushButton* againButton = decision.addButton(tr("Try again"), QMessageBox::ActionRole);
		decision.addButton(tr("Keep current data"), QMessageBox::RejectRole);
		decision.setIcon(QMessageBox::Warning);
		
		__timer.stop();
		
		decision.exec();
		
		if (decision.clickedButton() == againButton) {
			refreshData();
			return;
		} else
			return;
	}
	
	__userInterface->statusBarUpdate("Standby...");
	
	QString temp;
	if (__userInterface->getGLContext()->getTrackedPilot())
		temp = __userInterface->getGLContext()->getTrackedPilot()->callsign;
	
	__firsData->clearAll();
	
	__vatsimData->parseDataFile(_data);
	__userInterface->getClientsBox()->setText("Clients: " + QString::number(
			__vatsimData->getPilots().size() + __vatsimData->getATCs().size()
		));
	
	if (__userInterface->getGLContext()->getTrackedPilot())
		__userInterface->getGLContext()->getTrackedPilot() = __vatsimData->findPilot(temp);
	
	__userInterface->statusBarUpdate(
		"Last update: " + __vatsimData->getDateDataUpdated().toString("dd MMM yyyy, hh:mm") + " UTC"
	);
	
	emit dataUpdated();
}

void
VatsinatorApplication::__showDataAlert() {
	QMessageBox decision;
	decision.setText(tr("Vatsinator was unable to fetch Vatsim's data file."));
	decision.setInformativeText(tr("What do you want to do with that?"));
	QPushButton* againButton = decision.addButton(tr("Try again"), QMessageBox::ActionRole);
	decision.addButton(tr("Keep current data"), QMessageBox::RejectRole);
	decision.setIcon(QMessageBox::Warning);
	
	__timer.stop();
	
	decision.exec();
	
	if (decision.clickedButton() == againButton)
		refreshData();
}

void
VatsinatorApplication::__fetchStatusFile() {
	__userInterface->statusBarUpdate("Fetching data...");
	__httpHandler->fetchData(VATSIM_STATUS_URL);
}