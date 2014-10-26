/*
    plaintextdownloader.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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


#ifndef PLAINTEXTDOWNLOADER_H
#define PLAINTEXTDOWNLOADER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QQueue>
#include <QUrl>

/**
 * The PlainTextDownloader is used to download files directly into memory.
 */
class PlainTextDownloader : public QObject {
  Q_OBJECT

signals:
  /**
   * Emitted when download is complete.
   * 
   * \param data The fetch file content.
   */
  void finished(QString data);
  
  /**
   * Emitted if a network error occured during the download process.
   * 
   * \param error The error information.
   */
  void error(QString error);
  
  /**
   * This signal is emitted in order to indicate the progress.
   * 
   * \param read Bytes read out of _total_.
   * \param total Total bytes to be read.
   */
  void progress(qint64 read, qint64 total);

public:
  /**
   * The default constructor passes _parent_ to QObject.
   */
  PlainTextDownloader(QObject* parent = nullptr);

  /**
   * Enqueues the request.
   * If the queue is empty, the specified URL will be downloaded immediately.
   * 
   * \param url URL to be downloaded.
   */
  void fetch(const QUrl& url);
  
  /**
   * Returns false if the queue is empty, otherwise true.
   */
  inline bool hasPendingTasks() const { return !__urls.isEmpty(); }
  
  /**
   * Returns true if anything is being downloaded.
   */
  inline bool isWorking() const { return __reply != nullptr; }
  
public slots:
  /**
   * Aborts the download.
   * If the queue is not empty, new request will be started immediately.
   * The finished() signal will _not_ be emitted.
   */
  void abort();

private:
  void  __startRequest();
  
private slots:
  void __readyRead();
  void __finished();
  
private:
  QQueue<QUrl>    __urls;

  QString __temp;
  QString __data;

  QNetworkAccessManager __nam;

  QNetworkReply*  __reply;

};

#endif // PLAINTEXTDOWNLOADERR_H
