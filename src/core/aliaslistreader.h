/*
 * aliaslistreader.h
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

#ifndef CORE_ALIASLISTREADER_H
#define CORE_ALIASLISTREADER_H

#include "core/resourcefile.h"
#include "core/vtrcore_export.h"
#include <QtCore/QMap>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 *
 * The reader for list of aliases.
 */
class VTRCORE_EXPORT AliasListReader : public ResourceFile {
    Q_OBJECT

public:
    /**
     * Creates a new alias list reader. Passes \c parent to the QObject's
     * constructor.
     */
    explicit AliasListReader(QObject* parent = nullptr);
    
    /**
     * Returns a list of ICAO aliases.
     */
    QList<QString> aliases(const QString& key) const;
    
    /**
     * Returns all aliases in a map.
     */
    const QMap<QString, QString>& aliases() const { return m_aliases; }

private slots:
    void readData();
    
private:
    QMap<QString, QString> m_aliases;

};

}} /* namespace Vatsinator::Core */

#endif // CORE_ALIASLISTREADER_H
