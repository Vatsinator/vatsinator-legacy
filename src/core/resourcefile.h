/*
 * resourcefile.h
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

#ifndef CORE_RESOURCEFILE_H
#define CORE_RESOURCEFILE_H

#include "coreexport.h"
#include "filereader.h"

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 *
 * The ResourceFile class represents one of Vatsinator's manifest-controlled
 * files.
 */
class __VtrCoreApi__ ResourceFile : public FileReader {
    Q_OBJECT
    
    /**
     * Stores name of the file.
     */
    Q_PROPERTY(QString name MEMBER m_name)

public:
    explicit ResourceFile(const QString& name, QObject* parent = nullptr);
    
    /**
     * Returns the file contents.
     */
    const QByteArray& data() const { return m_data; }
    
protected:
    /**
     * \copydoc FileReader::read
     */
    void read(const QString& fileName) final override;
    
private:
    void addLocalPath();
    
    QString m_name;
    QByteArray m_data;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_RESOURCEFILE_H
