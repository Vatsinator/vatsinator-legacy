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
 * The VatsimDataReader class is a utility class that simplified the
 * process of fetching and parsing data files by VATSIM.
 * 
 * \sa VatsimDataDocument, VatsimStatusDocument and VatsimStatusReader.
 */
class __VtrCoreApi__ VatsimDataReader : public QObject {
    Q_OBJECT
    
signals:
    void dataRead(Vatsinator::Core::VatsimDataDocument data);

public:
    explicit VatsimDataReader(QObject* parent = nullptr);
    virtual ~VatsimDataReader();
    
    void read(const QUrl& url);
    
private:
    VatsimDataDocument parse(const QByteArray& data);
    
private:
    void read();
    
private:
    FileDownloader* m_downloader;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_VATSIMDATAREADER_H
