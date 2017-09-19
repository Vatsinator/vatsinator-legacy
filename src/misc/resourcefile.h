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

#ifndef MISC_RESOURCEFILE_H
#define MISC_RESOURCEFILE_H

#include "misc/filereader.h"
#include "misc/v8rmisc_export.h"

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Core
 * @{
 *
 * The ResourceFile class represents one of Vatsinator's manifest-controlled
 * files.
 */
class V8RMISC_EXPORT ResourceFile : public FileReader {
    Q_OBJECT
    
    /**
     * Stores name of the file.
     * This is the default property.
     */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    Q_CLASSINFO("DefaultProperty", "name")

signals:
    void nameChanged(const QString& name);

public:
    /**
     * Creates an empty \c ResourceFile instance.
     */
    explicit ResourceFile(QObject* parent = nullptr);

    /**
     * Creates a \c ResourceFile that immediately reads the specified file.
     */
    explicit ResourceFile(const QString& name, QObject* parent = nullptr);

    /**
     * Destroys this \c ResourceFile instance.
     */
    virtual ~ResourceFile();
    
    /**
     * Returns the file contents.
     */
    const QByteArray& data() const { return m_data; }

    const QString& name() const { return m_name; }
    void setName(const QString& name);
    
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

}} /* namespace Vatsinator::Misc */

Q_DECLARE_METATYPE(Vatsinator::Misc::ResourceFile*)

#endif // MISC_RESOURCEFILE_H
