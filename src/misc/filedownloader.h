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

#ifndef MISC_FILEDOWNLOADER_H
#define MISC_FILEDOWNLOADER_H

#include "misc/vtrmisc_export.h"
#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 * 
 * The FileDownloader class is a convenience class that simplfies
 * downloading files from the internet.
 */
class VTRMISC_EXPORT FileDownloader : public QObject {
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

    /**
     * Returns the number of retries.
     *
     * When this number of retries is reached, the \c error() signal is emitted.
     */
    int maxRetries() const { return m_maxRetries; }

    /**
     * Sets the max trials to the given value.
     */
    void setMaxRetries(int maxRetries);

private:
    void start(const QNetworkRequest& request);
    
private slots:
    void finish();
    
private:
    bool m_isDownloading = false;
    int m_maxRetries = 1;
    int m_retries = 0;
    QByteArray m_data;
    
}; /** @} */

}} /* namespace Vatsinator::Misc */

#endif // MISC_FILEDOWNLOADER_H
