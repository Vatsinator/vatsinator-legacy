/*
    MetarsWindow.cpp
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

#include "../include/MetarsWindow.h"

#include "../include/HttpHandler.h"
#include "../include/MetarsHandler.h"

MetarsWindow::MetarsWindow(QWidget* _parent) :
		QWidget(_parent) {
	
	setupUi(this);
	
	__setWindowPosition();
	
	__httpHandler = new HttpHandler(ProgressBar);
	
	__metarsHandler = new MetarsHandler(__httpHandler);
	
	connect(FetchButton,		SIGNAL(clicked()), this, SLOT(fetchMetar()));
	connect(RefreshAllButton,	SIGNAL(clicked()), this, SLOT(refreshAll()));
	connect(ClearButton,		SIGNAL(clicked()), this, SLOT(clear()));
	connect(__metarsHandler,	SIGNAL(newMetarsAvailable(const QVector<Metar>&)),
			this, SLOT(metarReceived(const QVector<Metar>&)));
	
	MetarICAO->setFocus();
}

void
MetarsWindow::showWindow() {
	this->show();
}

void
MetarsWindow::showWindow(QString _icao) {
	this->show();
	__metarsHandler->fetchMetar(_icao);
}

void
MetarsWindow::fetchMetar() {
	__metarsHandler->fetchMetar(MetarICAO->text());
	MetarICAO->setText("");
}

void
MetarsWindow::metarReceived(const QVector< Metar >& _metars) {
	MetarsDisplay->clear();
	
	for (const Metar& m: _metars) {
		if (!m.metar.isEmpty())
			MetarsDisplay->addItem(m.metar);
	}
}

void
MetarsWindow::refreshAll() {
	__metarsHandler->updateAllMetars();
}

void
MetarsWindow::clear() {
	__metarsHandler->clear();
}

void
MetarsWindow::close() {
	hide();
}

void
MetarsWindow::__setWindowPosition() {
	QDesktopWidget* desktop = QApplication::desktop();
	
	int screenWidth, width;
	int screenHeight, height;
	
	int x, y;
	
	QSize windowSize;
	
	screenWidth = desktop -> width();
	screenHeight = desktop -> height();
	
	windowSize = size();
	width = windowSize.width();
	height = windowSize.height();
	
	x = (screenWidth - width) / 2;
	y = (screenHeight - height) / 2;
	y -= 50;
	
	move(x, y);
}