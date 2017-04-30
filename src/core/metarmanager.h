/*
 * metarmanager.h
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

#ifndef CORE_METARMANAGER_H
#define CORE_METARMANAGER_H

#include "core/vtrcore_export.h"
#include "core/metar.h"
#include <QtCore/QObject>
#include <QtCore/QUrl>

namespace Vatsinator { namespace Core {

class FileDownloader;

/**
 * \ingroup Core
 * @{
 *
 * The MetarManager keeps list of all METARs.
 */
class VTRCORE_EXPORT MetarManager : public QObject {
    Q_OBJECT
    
signals:
    /**
     * Emitted when all METARs are downloaded and read, every 10 minutes.
     */
    void updated();

public:
    /**
     * Creates a new \c MetarManager instance, passes \c parent to \c QObject.
     */
    explicit MetarManager(QObject* parent = nullptr);
    
    /**
     * Sets the URL where all weather reports can be fetched from.
     */
    void setUrl(const QUrl& url);
    
    /**
     * Finds a METAR for the given ICAO code.
     */
    Metar find(const QString& icao) const;

    /**
     * Returns all METARs as a list.
     */
    const QList<Metar>& metars() const { return m_metars; }
    
private slots:
    void fetchMetars();
    void readAllMetars();
    
private:
    QUrl m_url;
    FileDownloader* m_downloader;
    QList<Metar> m_metars;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::MetarManager*)

#endif // CORE_METARMANAGER_H
