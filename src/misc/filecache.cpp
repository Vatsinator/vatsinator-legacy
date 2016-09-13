/*
 * filecache.cpp
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

#include "filecache.h"
#include <QtCore>

namespace {
    QString cacheDirectory()
    {
        static QString directory;
        if (directory.isEmpty()) {
            directory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        }
        
        return directory;
    }
}

namespace Vatsinator { namespace Misc {

bool FileCache::exists(const QString& fileName)
{
    QString filePath = cacheDirectory() % QDir::separator() % fileName;
    return QFileInfo::exists(filePath);
}

QString FileCache::path(const QString& fileName)
{
    return cacheDirectory() % QDir::separator() % fileName;
}

bool FileCache::moveToCache(const QString& originPath, const QString& targetPath)
{
    QFile file(originPath);
    if (!file.open(QIODevice::ReadWrite)) {
        qWarning("Failed caching %s: no access to the file", qPrintable(originPath));
        return false;
    }
    
    file.close();
    
    QString target = cacheDirectory() % QDir::separator() % targetPath;
    QFileInfo info(target);
    
    QDir dir = info.absoluteDir();
    if (!dir.exists())
        dir.mkpath(".");
    
    if (info.exists()) {
        QFile::remove(target);
    }
    
    bool result = file.rename(target);
    return result;
}

bool FileCache::storeInCache(const QString& targetPath, const QByteArray& data)
{
    QString target = cacheDirectory() % QDir::separator() % targetPath;
    QFileInfo info(target);
    
    QDir dir = info.absoluteDir();
    if (!dir.exists())
        dir.mkpath(".");
    
    if (info.exists()) {
        QFile::remove(target);
    }
    
    QFile file(info.absoluteFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Failed writing to %s: no access to the file", qPrintable(info.absoluteFilePath()));
        return false;
    }
    
    auto bytes = file.write(data);
    file.close();
    
    if (bytes < 0 || bytes < data.size())
        return false;
    else
        return true;
}

}} /* namespace Vatsinator::Misc */
