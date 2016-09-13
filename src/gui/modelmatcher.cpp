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

#include "modelmatcher.h"
#include <QtCore>

namespace Vatsinator { namespace Gui {

ModelMatcher::ModelMatcher() {}

ModelMatcher::~ModelMatcher() {}

void ModelMatcher::readFromJson(const QJsonDocument& json)
{    
    Q_ASSERT(json.isObject());
    QJsonObject object = json.object();
    
    QStringList keys = object.keys();
    std::for_each(keys.begin(), keys.end(), [this, &object](const QString& key) {
        QJsonValue value = object.value(key);
        Q_ASSERT(value.isArray());;
        QJsonArray array = value.toArray();
        
        std::for_each(array.begin(), array.end(), [this, &key](const QJsonValue& value) {
            Q_ASSERT(value.isString());
            QString mapping = value.toString();
            m_mappings.insert(mapping, key);
        });
    });
}

QString ModelMatcher::match(const QString& modelString) const
{
    if (modelString.isEmpty())
        return defaultModel();
    
    QRegExp rx("(\\w{3,})");
    if (rx.indexIn(modelString) == -1) {
        qWarning("No model match for %s ", qPrintable(modelString));
        return defaultModel();
    }
    
    return m_mappings.value(rx.cap(1), defaultModel());
}

QString ModelMatcher::defaultModel() const
{
    return QStringLiteral("B737");
}

}} /* namespace Vatsinator::Gui */
