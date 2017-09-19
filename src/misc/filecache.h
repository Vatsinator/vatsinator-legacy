/*
 * filecache.h
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

#ifndef MISC_FILECACHE_H
#define MISC_FILECACHE_H

#include "misc/v8rmisc_export.h"
#include <QtCore/QString>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 * 
 * The FileCache class makes it easy to keep files cached.
 */
class V8RMISC_EXPORT FileCache {
public:
    /**
     * Verifies whether the given file exists in the cache already or not.
     * 
     * \note \c fileName has to be a relative path.
     */
    static bool exists(const QString& fileName);
    
    /**
     * Returns absolute path to the file in cache.
     */
    static QString path(const QString& fileName);
    
    /**
     * Moves the given file to the cache directory.
     * 
     * \note If a file under \c targetPath already exists, it will be removed.
     * 
     * \return True if the file was successfully cached.
     */
    static bool moveToCache(const QString& originPath, const QString& targetPath);
    
    /**
     * Stores the given data in the cache file.
     * 
     * \note If a file under \c targetPath already exists, it will be removed.
     * 
     * \return True if the given \c data was successfully cached.
     */
    static bool storeInCache(const QString& targetPath, const QByteArray& data);
    
}; /** @} */

}} /* namespace Vatsinator::Misc */

#endif // MISC_FILECACHE_H
