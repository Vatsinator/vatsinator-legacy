/*
 * aliaslistreader.cpp
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

#include "aliaslistreader.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

AliasListReader::AliasListReader(QObject* parent) :
    ResourceFile(QStringLiteral("data/alias.json"), parent)
{
    readData();
    connect(this, &FileReader::fileRead, this, &AliasListReader::readData);
}

QList<QString> AliasListReader::aliases(const QString& key) const
{
    QList<QString> aliases = m_aliases.values(key);
    if (key.length() == 3)
        aliases.prepend(QStringLiteral("K") % key);
    
    return aliases;
}

void AliasListReader::readData()
{
    QJsonDocument document = QJsonDocument::fromJson(data());
    QJsonArray array = document.array();
    
    for (auto a: array) {
        QJsonObject object = a.toObject();
        QString target = object["target"].toString();
        QJsonArray aliases = object["alias"].toArray();
        
        for (auto b: aliases) {
            if (b.isString()) {
                m_aliases.insert(b.toString(), target);
            } else if (b.isObject()) {
                // TODO
            }
        }
    }
    
    qDebug("AliasListReader: read %i aliases", m_aliases.size());
}

}} /* namespace Vatsinator::Core */
