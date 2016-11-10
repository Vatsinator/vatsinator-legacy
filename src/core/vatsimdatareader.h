/*
 * vatsimdatareader.h
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

#ifndef CORE_VATSIMDATAREADER_H
#define CORE_VATSIMDATAREADER_H

#include "filedownloader.h"
#include "coreexport.h"
#include "vatsimdatadocument.h"
#include <QObject>
#include <QUrl>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 * 
 * The VatsimDataReader class is a utility class that simplifies the
 * process of fetching and parsing data files by VATSIM.
 * 
 * \sa VatsimDataDocument, VatsimStatusDocument and VatsimStatusReader.
 */
class __VtrCoreApi__ VatsimDataReader : public QObject {
    Q_OBJECT
    
signals:
    /**
     * Emitted each time when the data document is downloaded and
     * successfully parsed.
     */
    void dataRead(Vatsinator::Core::VatsimDataDocument data);

public:
    /**
     * Creates a new \c VatsimDataReader instance. Passes \c parent
     * to the QObject.
     */
    explicit VatsimDataReader(QObject* parent = nullptr);

    /**
     * Destroys this \c VatsimDataReader.
     */
    virtual ~VatsimDataReader();
    
    /**
     * Reads the data document under location specified by \c url.
     * If the \c url points to a local file, reads it immediately.
     * If the \c url points to a remote file, downloads it prior to reading it.
     */
    void read(const QUrl& url);

    /**
     * Picks a random URL from the \c urlChain and calls \ref read(const QUrl&)
     * on it. If it fails, picks another one and does that forever until any
     * document is downloaded and parsed successfully.
     */
    void read(const QList<QUrl>& urlChain);
    
private:
    VatsimDataDocument parse(const QByteArray& data);
    
private slots:
    void read();
    void readAnotherOneFromChain();
    
private:
    FileDownloader* m_downloader;
    QList<QUrl> m_urls;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_VATSIMDATAREADER_H
