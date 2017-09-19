/*
 * vatsimstatusreader.h
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

#ifndef CORE_VATSIMSTATUSREADER_H
#define CORE_VATSIMSTATUSREADER_H

#include "core/filedownloader.h"
#include "core/vatsimstatusdocument.h"
#include "core/v8rcore_export.h"
#include <QtCore/QObject>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 * 
 * The VatsimStatusReader class is responsible for reading the status.txt
 * file, fetching it before, if needed.
 * 
 * \sa VatsimStatusDocument, VatsimDataReader and VatsimDataDocument.
 */
class V8RCORE_EXPORT VatsimStatusReader : public QObject {
    Q_OBJECT
    
signals:
    /**
     * Emitted when the document is fetched (if loaded from the remote location),
     * read and parsed.
     */
    void statusRead(Vatsinator::Core::VatsimStatusDocument status);

public:
    /**
     * Creates a new VatsimStatusReader instance, passes \c parent to
     * the \c QObject.
     */
    explicit VatsimStatusReader(QObject* parent = nullptr);
    
    virtual ~VatsimStatusReader();
    
public slots:
    /**
     * Downloads the status from the internet (if the specified URL is a remote
     * one) and loads it. Loaded document will be passed in the \c statusRead()
     * signal.
     * 
     * \param url The URL of the status.txt file.
     */
    void read(const QUrl& url);
    
private:
    VatsimStatusDocument parse(const QByteArray& data);
    
private slots:
    void read();
    
private:
    FileDownloader* m_downloader;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_VATSIMSTATUSREADER_H
