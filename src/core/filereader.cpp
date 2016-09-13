/*
 * filereader.cpp
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

#include "filereader.h"
#include "filedownloader.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

FileReader::FileReader(QObject* parent) : QObject(parent)
{
    connect(this, &FileReader::urlChanged, this, &FileReader::update);
}

FileReader::FileReader(const QString& localFilePath, const QUrl& url, QObject* parent) :
    FileReader(parent)
{
    read(localFilePath);
    setUrl(url);
}

void FileReader::setUrl(const QUrl& url)
{
    if (m_url != url) {
        m_url = url;
        emit urlChanged(m_url);
    }
}

void FileReader::read(const QString& fileName)
{
    setRead(true);
    Q_UNUSED(fileName);
}

void FileReader::setRead(bool read)
{
    m_read = read;
    if (m_read)
        emit fileRead();
}

void FileReader::update()
{
    if (!m_url.isValid())
        return;
    
    if (!m_downloader) {
        m_downloader = new FileDownloader(this);
        connect(m_downloader, &FileDownloader::finished, this, &FileReader::downloadFinished);
        // TODO Handle errors
    }
    
    m_downloader->fetch(m_url);
}

void FileReader::downloadFinished(QUrl url)
{
    Q_UNUSED(url);
    // TODO
}

}} /* namespace Vatsinator::Core */
