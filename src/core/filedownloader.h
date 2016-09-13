/*
 * filedownloader.h
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef CORE_FILEDOWNLOADER_H
#define CORE_FILEDOWNLOADER_H

#include "coreexport.h"
#include <QByteArray>
#include <QNetworkReply>
#include <QObject>
#include <QUrl>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 * 
 * The FileDownloader class is a convenience class that simplfies
 * downloading files from the internet.
 */
class __VtrCoreApi__ FileDownloader : public QObject {
    Q_OBJECT
    
signals:
    /**
     * Emited when download is complete.
     *
     * \param url The request URL.
     */
    void finished(QUrl url);
    
    /**
     * Emited when an error occurs.
     * \param error Error string.
     * \param url URL of the request.
     */
    void error(QString error, QUrl url);
    
    /**
     * Emited to notify about the download progress.
     */
    void progress(quint64 received, quint64 total);
    
public:
    /**
     * Creates new FileDownloader instance. Passes \c parent to the
     * QObject's constructor.
     */
    explicit FileDownloader(QObject* parent = nullptr);
    
    /**
     * Destroys the FileDownloader instance.
     */
    virtual ~FileDownloader();
    
    /**
     * Fetches the file under the URL. If the downloader is working at the moment,
     * it will be aborted.
     * 
     * \sa isDownloading().
     */
    void fetch(const QUrl& url);
    
    /**
     * Returns \c true if the FileDownloader is currently downloading
     * something.
     */
    bool isDownloading() const { return m_isDownloading; }
    
    /**
     * Access downloaded data.
     */
    const QByteArray& data() const { return m_data; }
    
private slots:
    void finish();
    
private:
    bool m_isDownloading = false;
    QByteArray m_data;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_FILEDOWNLOADER_H
