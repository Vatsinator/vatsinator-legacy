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
#include <QSharedPointer>
#include <QObject>
#include <QQueue>
#include <QFile>
#include <QUrl>

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
     * \param url The request URL.
     */
    void finished(QString fileName, QUrl url);
    
    /**
     * Emited when an error occurs.
     * \param error Error string.
     * \param url URL of the request.
     */
    void error(QString error, QUrl url);
    
public:
    /**
     * Creates new FileDownloader instance.
     */
    FileDownloader(QObject* parent = nullptr);
    
    virtual ~FileDownloader();
    
    /**
     * If the requests queue is empty, downloads the given file
     * immediately. Otherwise, enqueues the url.
     */
    void fetch(QUrl url);
    
    /**
     * Gets number of tasks in the queue.
     */
    inline int tasks() const
    {
        return __urls.size();
    }
    
    /**
     * Returns true if there are any queries scheduled.
     * This function is the same as calling
     * 
     * \code{.cpp}
     * tasks() > 0
     * \endcode
     */
    inline bool anyTasksLeft() const
    {
        return !__urls.isEmpty();
    }
    
private slots:
    void __fetchImpl(QUrl url);
    void __readyRead();
    void __finished();
    
private:
    void __startRequest();
    
    QQueue<QUrl> __urls;
    QFile __output;
    
    QSharedPointer<QNetworkAccessManager> __nam;
    QNetworkReply*        __reply;
};

#endif // FILEDOWNLOADER_H
