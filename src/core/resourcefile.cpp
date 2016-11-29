/*
 * resourcefile.cpp
 * Copyright (C) 2016  Michał Garapich
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

#include "resourcefile.h"
#include "config.h"
#include <QtCore>

namespace {

QString findFilePathImpl(const QList<QString> filePaths)
{
    for (auto fp: filePaths) {
        if (QFile::exists(fp))
            return fp;
    }
    
    return filePaths.first();
}

QStringList filesInSourcePath(const QString& fileName)
{
    QStringList dirs = QString(VATSINATOR_SOURCE_PATH).split(";");
    for (QString& d: dirs) {
        d += QStringLiteral("/resources/") % fileName;
    }
    
    return dirs;
}

}

namespace Vatsinator { namespace Core {

ResourceFile::ResourceFile(QObject *parent) : FileReader(parent) {}

ResourceFile::ResourceFile(const QString& name, QObject* parent) :
    FileReader(parent),
    m_name(name)
{
    addLocalPath();
}

ResourceFile::~ResourceFile() {}

void ResourceFile::setName(const QString &name)
{
    m_name = name;
    addLocalPath();
}

void ResourceFile::read(const QString& fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        m_data = file.readAll();
        file.close();
        
        setRead(true);
    } else {
        qWarning("Failed to open %s", qPrintable(fileName));
    }
}

void ResourceFile::addLocalPath()
{
    QList<QString> filePaths;
    filePaths << m_name;
    
    filePaths.append(filesInSourcePath(m_name));
    
#if defined(Q_OS_ANDROID)
    filePaths << QStringLiteral("assets:/") + m_name;
#elif defined(Q_OS_LINUX)
    filePaths << QStringLiteral(VATSINATOR_PREFIX) + m_name;
#elif defined(Q_OS_MAC)
    filePaths << QCoreApplication::applicationDirPath() % QStringLiteral("../Resources/") + m_name;
#endif
    
    QString localPath = findFilePathImpl(filePaths);
    qDebug("\"%s\" loaded from %s", qPrintable(m_name), qPrintable(localPath));
    read(localPath);
}

}} /* namespace Vatsinator::Core */
