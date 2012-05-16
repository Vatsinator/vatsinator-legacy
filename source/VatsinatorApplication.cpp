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
#include "db/WorldMap.h"

#include "modules/FlightTracker.h"
#include "modules/ModulesManager.h"

#include "network/HttpHandler.h"

#include "settings/SettingsManager.h"

#include "ui/UserInterface.h"
#include "ui/windows/SettingsWindow.h"

#include "vatsimdata/VatsimDataHandler.h"

#include "VatsinatorApplication.h"
#include "defines.h"

VatsinatorApplication::VatsinatorApplication(int& _argc, char** _argv) :
		QApplication(_argc, _argv),
		__airportsData(new AirportsDatabase),
		__firsData(new FirsDatabase),
		__worldMap(new WorldMap),
		__vatsimData(new VatsimDataHandler),
		__settingsManager(new SettingsManager),
		__modulesManager(new ModulesManager),
		__userInterface(new UserInterface) {
	
#ifndef NO_DEBUG
	std::cout << "VATSINATOR_DAT: " << VATSINATOR_DAT << std::endl;
	
#ifdef VATSINATOR_PLATFORM_LINUX
#pragma message("Platform: Linux")
#elif defined VATSINATOR_PLATFORM_WIN32
#pragma message("Platform: Win32")
#else
#pragma message("Platform: undefined")
#endif
	
	std::cout << "sizeof(unsigned short) = " << sizeof(unsigned short) << std::endl;

#endif
	
	// destroy all children windows before the program exits
	connect(this,			SIGNAL(destroyed()),
		__userInterface,	SLOT(hideAllWindows()));
	
	// SettingsManager instance is now created, let him get the pointer & connect his slots
	__settingsManager->init();
	__modulesManager->init();
	
	// connect data refresher with the timer
	connect(&__timer, SIGNAL(timeout()), this, SLOT(refreshData()));
	
	// read database
	__airportsData->init();
	
	// read .dat file
	__vatsimData->init();
	
	// if fetch goes wrong, show the alert
	connect(__vatsimData, SIGNAL(dataCorrupted()), this, SLOT(__showDataAlert()));
	
	// show main window
	__userInterface->show();
	
	// create something that will handle our http requests
	__httpHandler = new HttpHandler(__userInterface->getProgressBar());
	
	// connect first finished signal to statusFileUpdate
	connect(__httpHandler, SIGNAL(finished(QString)), this, SLOT(__statusFileUpdated(QString)));
	
	// fetch the status file
	__fetchStatusFile();
	
	// start the timer
	__timer.start(__settingsManager->getRefreshRate() * 60000);
}

VatsinatorApplication::~VatsinatorApplication() {
	delete __settingsManager;
	delete __httpHandler;
	delete __vatsimData;
	delete __airportsData;
	delete __firsData;
	delete __worldMap;
	delete __userInterface;
	delete __modulesManager;
	
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
	__timer.start(__settingsManager->getRefreshRate() * 60000);
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
	
	__firsData->clearAll();
	
	__vatsimData->parseDataFile(_data);
	__userInterface->getClientsBox()->setText(static_cast< QString >("Clients: ") % QString::number(
			__vatsimData->getPilots().size() + __vatsimData->getATCs().size()
		) % " (" % QString::number(__vatsimData->getPilots().size()) % " pilots, " %
			QString::number(__vatsimData->getATCs().size()) % " ATCs)"
		);
	
	__userInterface->statusBarUpdate(
		"Last update: " + __vatsimData->getDateDataUpdated().toString("dd MMM yyyy, hh:mm") + " UTC"
	);
	
	// we cannot depend on signals & slots system here, as GLrepaint() would be called
	// earlier, causing segfault
	FlightTracker::GetSingleton().updateData();
	
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