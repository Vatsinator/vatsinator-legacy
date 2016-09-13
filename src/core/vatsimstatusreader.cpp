/*
 * vatsimstatusreader.cpp
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

#include "vatsimstatusreader.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

VatsimStatusReader::VatsimStatusReader(QObject* parent) :
    QObject(parent),
    m_downloader(new FileDownloader(this))
{
    connect(m_downloader, &FileDownloader::finished, this,
            static_cast<void (VatsimStatusReader::*)()>(&VatsimStatusReader::read));
}

VatsimStatusReader::~VatsimStatusReader() {}

void VatsimStatusReader::read(const QUrl& url)
{
    if (url.isLocalFile()) {
        QFile file(url.toLocalFile());
        if (file.open(QIODevice::ReadOnly)) {
            VatsimStatusDocument s = parse(file.readAll());
            file.close();
            emit statusRead(s);
        } else {
            qWarning("VatsimStatusReader: could not open the file %s for reading!", qPrintable(url.toLocalFile()));
        }
    } else {
        m_downloader->fetch(url);
    }
}

void VatsimStatusReader::read()
{
    VatsimStatusDocument s = parse(m_downloader->data());
    emit statusRead(s);
}

VatsimStatusDocument VatsimStatusReader::parse(const QByteArray& data)
{
    if (data.isEmpty())
        return VatsimStatusDocument();
    
    VatsimStatusDocument s;
    QList<QUrl> dataFileUrls, serverListUrls;
    
    static QRegExp rx("(msg0|url0|url1|moveto0|metar0)=(.+)\\b");
    QStringList tempList = QString(data).split(QRegExp("\r?\n"), QString::SkipEmptyParts);
    for (auto& temp: tempList) {
        if (temp.startsWith(';'))
            continue;
        
        if (rx.indexIn(temp) >= 0) {
            QString key = rx.cap(1);
            QString value = rx.cap(2);
            
            if (key == "moveto0") {
                s.setMoveTo(value);
            } else if (key == "metar0") {
                s.setMetar(value);
            } else if (key == "url0") {
                dataFileUrls << QUrl(value);
            } else if (key == "url1") {
                serverListUrls << QUrl(value);
            } else if (key == "msg0") {
                s.setMessage(value);
            }
        }
    }
    
    s.setDataFileUrls(dataFileUrls);
    s.setServersListUrls(serverListUrls);
    
    return s;
}

}} /* namespace Vatsinator::Core */
