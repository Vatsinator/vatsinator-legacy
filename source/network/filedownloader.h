/*
    filedownloader.h
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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


#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QQueue>
#include <QFile>
#include <QUrl>

class QProgressBar;
class QNetworkReply;

/**
 * The FileDownloader class is a convenience class that simplfies
 * downloading files from the internet.
 *
 * \todo Add progress() signal.
 */
class FileDownloader : public QObject {
    Q_OBJECT
    
signals:
    /**
     * Emited when download is complete.
     *
     * \param fileName Location of the downloaded file.
     */
    void finished(QString fileName);
    
    /**
     * Emited when an error occurs.
     * \param error Error string.
     */
    void error(QString error);
    
public:
    /**
     * Creates new FileDownloader instance.
     *
     * \param pb ProgressBar that will be updated during file fetching.
     * \param parent Passed to QObject.
     * \todo Remove pb.
     */
    FileDownloader(QProgressBar* pb = 0, QObject* parent = nullptr);
    
    /**
     * If the requests queue is empty, downloads the given file
     * immediately. Otherwise, enqueues the url.
     */
    void fetch(const QUrl& url);
    
    /**
     * Generates the temporary file name (with the absolute path)
     * from the given url.
     *
     * \todo Move to private scope.
     */
    QString fileNameForUrl(const QUrl& url);
    
    /**
     * Returns true if there are any queries left in the queue.
     */
    inline bool hasPendingTasks() const
    {
        return !__urls.isEmpty();
    }
    
private:
    void __startRequest();
    
private slots:
    void __readyRead();
    void __finished();
    void __updateProgress(qint64 read, qint64 total);
    
private:
    QQueue<QUrl>   __urls;
    QProgressBar*  __pb;
    QFile          __output;
    
    QNetworkAccessManager __nam;
    QNetworkReply*        __reply;
    
};

#endif // FILEDOWNLOADER_H
