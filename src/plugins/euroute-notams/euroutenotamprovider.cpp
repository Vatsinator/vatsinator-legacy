/*
 * euroutenotamprovider.cpp
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#include "euroutenotamprovider.h"
#include "core/filedownloader.h"
#include "misc/filecache.h"
#include <QtCore>

using namespace Vatsinator::Core;
using namespace Vatsinator::Misc;

namespace {
    constexpr auto EurouteCacheFileName = "euroute-notams.xml";
    constexpr auto EurouteNotamXmlUrl = "http://notams.euroutepro.com/notams.xml";
}

EurouteNotamProvider::EurouteNotamProvider(QObject* parent) :
    QObject(parent)
{
    if (!FileCache::exists(EurouteCacheFileName)) {
        downloadXml();
    } else {
        parseNotamFile(FileCache::path(EurouteCacheFileName));
        if (!isUpToDate())
            downloadXml();
    }
}

const NotamReply* EurouteNotamProvider::fetchNotams(const QString& icao)
{
    NotamReply* reply = new NotamReply(icao, this);
    
    if (m_notamsReady) {
        auto notams = m_notams.values(icao);
        for (const Notam& n: notams)
            reply->addNotam(n);
        
        reply->setFinished(true);
    } else {
        m_requests.append(reply);
    }
    
    return reply;
}

void EurouteNotamProvider::downloadXml()
{
    FileDownloader* fd = new FileDownloader(this);
    connect(fd, &FileDownloader::finished, this, &EurouteNotamProvider::parseNotams);
    fd->fetch(QUrl(EurouteNotamXmlUrl));
}

bool EurouteNotamProvider::isUpToDate()
{
    if (!m_updated.isValid() || m_updated.secsTo(QDateTime::currentDateTimeUtc()) > 24 * 60 * 60) // 24h
        return false;
    else
        return true;
}

void EurouteNotamProvider::finishRequests()
{
    for (NotamReply* reply: m_requests) {
        auto notams = m_notams.values(reply->icao());
        for (const Notam& n: notams)
            reply->addNotam(n);
        
        reply->setFinished(true);
    }
    
    m_requests.clear();
}

void EurouteNotamProvider::parseNotamFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("EurouteNotamProvider: failed to open %s for reading", qPrintable(file.fileName()));
        m_updated = QDateTime();
        return;
    }
    
    QXmlStreamReader xml(&file);
    parseNotamData(xml);
    
    file.close();
    finishRequests();
}

void EurouteNotamProvider::parseNotamData(QXmlStreamReader& xml)
{
    m_notams.clear();
    
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement && xml.name() == "general") {
            QXmlStreamAttributes attrs = xml.attributes();
            if (attrs.hasAttribute("last_update")) {
                QString dt = attrs.value("last_update").toString();
                m_updated = QDateTime::fromString(dt, "yyyy-MM-dd hh:mm:ss");
                qDebug("EUroute last update: %s", qPrintable(m_updated.toString()));
            }
        } else if (token == QXmlStreamReader::StartElement && xml.name() == "notam") {
            QXmlStreamAttributes attrs = xml.attributes();
            Q_ASSERT(attrs.hasAttribute("ident"));
            Q_ASSERT(attrs.hasAttribute("A"));
            
            Notam notam(attrs.value("ident").toString());
            notam.setIcao(attrs.value("A").toString());
            
            if (attrs.hasAttribute("B")) {
                QDateTime from = QDateTime::fromString(attrs.value("B").toString(), "yyyy-MM-dd hh:mm:ss");
                notam.setFrom(from);
            }
            
            if (attrs.hasAttribute("C_flag")) {
                QString cflag = attrs.value("C_flag").toString();
                if (cflag.isEmpty())
                    notam.setCflag(Notam::None);
                else if (cflag == "EST")
                    notam.setCflag(Notam::Est);
                else if (cflag == "PERM")
                    notam.setCflag(Notam::Perm);
            }
            
            if (attrs.hasAttribute("C")) {
                QDateTime to = QDateTime::fromString(attrs.value("C").toString(), "yyyy-MM-dd hh:mm:ss");
                notam.setTo(to);
            }
            
            if (attrs.hasAttribute("D")) {
                QString diurnal = attrs.value("D").toString();
                notam.setDiurnal(diurnal);
            }
            
            if (attrs.hasAttribute("E")) {
                QString msg = attrs.value("E").toString();
                notam.setNotam(msg);
            }
            
            if (attrs.hasAttribute("url")) {
                QString url = attrs.value("url").toString();
                notam.setUrl(url);
            }
            
            if (attrs.hasAttribute("type")) {
                QString type = attrs.value("type").toString();
                if (type == "R")
                    notam.setType(Notam::Replacement);
                else if (type == "C")
                    notam.setType(Notam::Cancellation);
                else
                    notam.setType(Notam::New);
            }
            
            m_notams.insert(notam.icao(), notam);
        }
    }
    
    m_notamsReady = true;
    qDebug() << "EURoute: read" << m_notams.size() << "NOTAMs";
}

void EurouteNotamProvider::parseNotams()
{
    FileDownloader* fd = qobject_cast<FileDownloader*>(sender());
    Q_ASSERT(fd);
    
    QByteArray data = fd->data();
    QXmlStreamReader xml(data);
    parseNotamData(xml);
    
    fd->deleteLater();
    finishRequests();
    
    FileCache::storeInCache(EurouteCacheFileName, data);
}
