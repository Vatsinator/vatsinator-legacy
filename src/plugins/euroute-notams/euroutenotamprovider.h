/*
 * euroutenotamprovider.h
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

#ifndef EUROUTENOTAMPROVIDER_H
#define EUROUTENOTAMPROVIDER_H

#include "misc/notamprovider.h"
#include <QMultiMap>
#include <QObject>
#include <QXmlStreamReader>

/**
 * \defgroup euroute-notams EUroute NOTAM provider.
 * @{
 * 
 * http://notams.euroutepro.com/doc/
 * 
 * To avoid excessive downloads, the XML file is cached and
 * fetched only once per 24h.
 */
class EurouteNotamProvider : public QObject, public Vatsinator::Misc::NotamProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.vtrmisc.NotamProvider" FILE "euroutenotamprovider.json")
    Q_INTERFACES(Vatsinator::Misc::NotamProvider)

public:
    explicit EurouteNotamProvider(QObject* parent = nullptr);
    
    /**
     * \copydoc Vatsinator::Misc::NotamProvider::fetchNotams
     */
    const Vatsinator::Misc::NotamReply* fetchNotams(const QString& icao) override;
    
private:
    void downloadXml();
    bool isUpToDate();
    void finishRequests();
    void parseNotamFile(const QString& filePath);
    void parseNotamData(QXmlStreamReader& xml);
    
private slots:
    void parseNotams();
    
private:
    bool m_notamsReady = false;
    QDateTime m_updated;
    QMultiMap<QString, Vatsinator::Misc::Notam> m_notams;
    QList<Vatsinator::Misc::NotamReply*> m_requests;
    
}; /** @} */

#endif // EUROUTENOTAMPROVIDER_H
