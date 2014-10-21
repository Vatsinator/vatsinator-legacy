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
#include <QProgressBar>
#include <QObject>
#include <QQueue>
#include <QUrl>

/**
 * The PlainTextDownloader is used to download files directly into memory.
 * 
 * \todo Create progress signal.
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
   */
  void error();
  
  /**
   * Emitted when user aborts the download.
   */
  void aborted();

public:
  /**
   * \todo Remove pb.
   * 
   * \param parent Passed to QObject.
   */
  PlainTextDownloader(QProgressBar* pb = 0, QObject* parent = nullptr);

  /**
   * Enqueues the request.
   * If the queue is empty, the specified URL will be downloaded immediately.
   * 
   * \param url URL to be downloaded.
   */
  void fetch(const QUrl& url);
  
  /**
   * \deprecated
   * Sets the given progress bar.
   */
  void setProgressBar(QProgressBar* pb);
  
  /**
   * The progress bar to be updated on readyRead().
   */
  inline QProgressBar* progressBar() { return __progressBar; }
  
  /**
   * The progress bar to be updated on readyRead().
   */
  inline const QProgressBar* progressBar() const { return __progressBar; }
  
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
   */
  void abort();

private:
  void  __startRequest();
  
private slots:
  void __readyRead();
  void __finished();
  void __updateProgress(qint64 read, qint64 total);
  
private:
  QProgressBar*   __progressBar;

  QQueue<QUrl>    __urls;

  QString __temp;
  QString __data;

  QNetworkAccessManager __nam;

  QNetworkReply*  __reply;

};

#endif // PLAINTEXTDOWNLOADERR_H
