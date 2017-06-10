/*
 * filereader.h
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#ifndef MISC_FILEREADER_H
#define MISC_FILEREADER_H

#include "misc/vtrmisc_export.h"
#include <QtCore/QObject>
#include <QtCore/QUrl>

namespace Vatsinator { namespace Misc {

class FileDownloader;

/**
 * \ingroup Misc
 * @{
 *
 * The FileReader class is a base class for every data file that is used
 * by Vatsinator. The file can be local (i.e. installed with the application)
 * as well as remote (downloaded from the internet and updated when needed).
 */
class VTRMISC_EXPORT FileReader : public QObject {
    Q_OBJECT
    
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)

signals:
    /**
     * The fileRead() signal is emitted each time the file is successfully
     * read.
     */
    void fileRead();
    
    void urlChanged(QUrl url);

public:
    /**
     * Creates a new file reader.
     */
    explicit FileReader(QObject* parent = nullptr);
    
    /**
     * Creates a new file reader that firstly reads the local file and then
     * downloads one from the URL provided.
     */
    explicit FileReader(const QString& localFilePath, const QUrl& url,
                        QObject* parent = nullptr);
    
    /**
     * Returns \c true if the file was read and contains correct data.
     */
    bool isRead() const { return m_read; }
    
    const QUrl& url() const { return m_url; }
    void setUrl(const QUrl& url);
    
protected:
    /**
     * Parses the file. After parsing, the \c fileRead() signal should be
     * emitted. If the file seems to be outdated, it can be signalized by
     * calling `setUpdated(false)`.
     * 
     * Override this method to parse custom file format.
     */
    virtual void read(const QString& fileName);
    
    void setRead(bool read);
    
private slots:
    void update();
    void downloadFinished(QUrl url);
    
private:
    bool m_read = false;
    QUrl m_url;
    FileDownloader* m_downloader = nullptr;
    
}; /** @} */

}} /* namespace Vatsinator::Misc */

#endif // MISC_FILEREADER_H
