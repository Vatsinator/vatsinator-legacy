/*
 * modelmatcher.cpp
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

#include <QtGui>

#include "storage/filemanager.h"

#include "modelmatcher.h"

ModelMatcher::ModelMatcher(QObject* parent) : QObject(parent)
{
    __readModels();
    __loadPixmaps();
}

QPixmap&
ModelMatcher::match(const QString& acft)
{
    if (acft.isEmpty())
        return __modelsPixmaps["1p"];
        
    for (auto pair : __modelsIds) {
        if (acft.contains(pair.first))
            return __modelsPixmaps[pair.second];
    }
    
    return __modelsPixmaps["1p"];
}

void
ModelMatcher::__readModels()
{
    __modelsIds << qMakePair(QString("ZZZZ"), QString("1p"));
    
    QFile modelsFile(FileManager::path("data/model"));
    
    if (!modelsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        notifyWarning(tr("File %1 could not be opened! Check file permissions or reinstall the application.").arg(modelsFile.fileName()));
        return;
    }
    
    while (!modelsFile.atEnd()) {
        QString line(modelsFile.readLine());
        line = line.simplified();
        
        if (line.startsWith('#') || line.isEmpty())
            continue;
            
        auto split = line.split(' ');
        __modelsIds << qMakePair(split[0], split[1]);
    }
}

void
ModelMatcher::__loadPixmaps()
{
    for (auto pair : __modelsIds) {
        if (!__modelsPixmaps.contains(pair.second)) {
            QString mPath = ":/pixmaps/model_" % pair.second % "32.png";
            __modelsPixmaps.insert(pair.second, QPixmap(mPath));
            Q_ASSERT(!__modelsPixmaps[pair.second].isNull());
        }
    }
}

