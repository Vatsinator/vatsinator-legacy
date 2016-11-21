/*
 * vatsimdatareader.cpp
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

#include "vatsimdatareader.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

VatsimDataReader::VatsimDataReader(QObject* parent) :
    QObject(parent),
    m_downloader(new FileDownloader(this))
{
    connect(m_downloader, &FileDownloader::finished, this,
            static_cast<void (VatsimDataReader::*)()>(&VatsimDataReader::read));
    connect(m_downloader, &FileDownloader::error, this, &VatsimDataReader::readAnotherOneFromChain);
}

VatsimDataReader::~VatsimDataReader() {}

void VatsimDataReader::read(const QUrl& url)
{
    if (url.isLocalFile()) {
        QFile file(url.toLocalFile());
        if (file.open(QIODevice::ReadOnly)) {
            VatsimDataDocument s = parse(file.readAll());
            file.close();
            emit dataRead(s);
        } else {
            qWarning("VatsimDataReader: could not open the file %s for reading!", qPrintable(url.toLocalFile()));
            readAnotherOneFromChain();
        }
    } else {
        m_downloader->fetch(url);
    }
}

void VatsimDataReader::read(const QList<QUrl>& urlChain)
{
    m_urls.append(urlChain);

    if (!m_downloader->isDownloading()) {
        int serverNo = qrand() % m_urls.length();
        QUrl url = m_urls.at(serverNo);
        read(url);
    }
}

void VatsimDataReader::read()
{
    VatsimDataDocument s = parse(m_downloader->data());
    emit dataRead(s);
}

void VatsimDataReader::readAnotherOneFromChain()
{
    if (m_urls.size() > 0) {
        int serverNo = qrand() % m_urls.length();
        QUrl url = m_urls.at(serverNo);
        read(url);
    }
}

VatsimDataDocument VatsimDataReader::parse(const QByteArray& data)
{
    if (data.isEmpty()) {
        qWarning("Trying to parse an empty document!");
        return VatsimDataDocument();
    }
    
    QStringList tempList = QString(data).split(QRegExp("\r?\n"), QString::SkipEmptyParts);
    enum { None, General, Clients, Prefile, Servers, VoiceServers } section = None;
    int invalidClients = 0;
    VatsimDataDocument document;
    
    std::for_each(tempList.begin(), tempList.end(), [&](auto line) {
        if (line.startsWith(';')) // comment
            return;
        
        if (line.startsWith('!')) { // section
            section = None;
            
            if (line.contains("GENERAL"))
                section = General;
            else if (line.contains("CLIENTS"))
                section = Clients;
            else if (line.contains("PREFILE"))
                section = Prefile;
            else if (line.contains("SERVERS"))
                section = Servers;
            else if (line.contains("VOICE SERVERS"))
                section = VoiceServers;
            else
                qWarning("VatsimDataReader: unknown section: %s", qPrintable(line));
                
            return;
        }
        
        switch (section) {
            case General: {
                static QRegExp rx("^\\b(UPDATE|RELOAD|CONNECTED CLIENTS)\\b\\s?=\\s?\\b(.+)\\b$");
                if (rx.indexIn(line) >= 0) {
                    QString key = rx.cap(1);
                    QString value = rx.cap(2);
                    
                    if (key == QStringLiteral("UPDATE")) {
                        document.setUpdate(QDateTime::fromString(value, "yyyyMMddhhmmss"));
                    } else if (key == QStringLiteral("RELOAD")) {
                        bool ok;
                        int reload = value.toInt(&ok);
                        Q_ASSERT(ok);
                        Q_UNUSED(ok);
                        document.setReload(reload);
                    } else if (key == QStringLiteral("CONNECTED CLIENTS")) {
                        bool ok;
                        int connectedClients = value.toInt(&ok);
                        Q_ASSERT(ok);
                        Q_UNUSED(ok);
                        document.setConnectedClients(connectedClients);
                    }
                }
                break;
            }
            
            case Clients: {
                ClientData client = ClientData::fromWhazzUpLine(line);
                if (client.valid()) {
                    document.addClient(client);
                } else {
                    qDebug("VatsimDataReader: error parsing line: %s", qPrintable(line));
                    invalidClients += 1;
                }
                break;
            }
            
            case Prefile: {
                ClientData client = ClientData::fromWhazzUpLine(line);
                if (client.valid()) {
                    document.addPrefiled(client);
                } else {
                    qDebug("VatsimDataReader: error parsing line: %s", qPrintable(line));
                    invalidClients += 1;
                }
                break;
            }
            
            default:
                break;
        }
    });
    
    m_document = document;
    return document;
}

}} /* namespace Vatsinator::Core */
