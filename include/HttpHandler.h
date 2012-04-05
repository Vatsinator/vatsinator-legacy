/*
    HttpHandler.h
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


#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <QNetworkAccessManager>
#include <QProgressBar>
#include <QObject>
#include <QQueue>
#include <QUrl>

class HttpHandler :
		public QObject {
	
	Q_OBJECT
	
public:
	
	HttpHandler(QProgressBar*, QObject* = 0);
	
	void fetchData(const QString&);
	
signals:
	void finished(QString);
	
private slots:
	void __readyRead();
	void __finished();
	void __updateProgress(qint64, qint64);
	
private:
	void	__startRequest();
	
	QProgressBar *	__progressBar;
	
	QQueue< QUrl >	__urls;
	
	QString __data;
	
	QNetworkAccessManager	__nam;

	QNetworkReply*	__reply;
	
};

#endif // HTTPHANDLER_H
