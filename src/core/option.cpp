/*
 * option.cpp
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

#include "option.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

class __VtrCoreHide__ OptionPrivate {
public:
    static void optionCreated(QString key, Option* instance)
    {
        m_options.insert(key, instance);
    }

    static void optionChanged(QString key, const QVariant& value)
    {
        QList<Option*> options = m_options.values(key);
        for (Option* o: options)
            QMetaObject::invokeMethod(o, "setValueImpl", Q_ARG(QVariant, value));
    }

    static void optionDestroyed(QString key, Option* instance)
    {
        m_options.remove(key, instance);
    }

private:
    static QMultiMap<QString, Option*> m_options;
};

QMultiMap<QString, Option*> OptionPrivate::m_options;


Option::Option(const QString& key, QObject* parent) :
    QObject(parent),
    m_key(key)
{
    OptionPrivate::optionCreated(m_key, this);
}

Option::Option(const QString& key, const QVariant& defaultValue, QObject* parent) :
    Option(key, parent)
{
    m_value = defaultValue;
}

Option::~Option()
{
    OptionPrivate::optionDestroyed(m_key, this);
}

void Option::track(QObject* target, const char* propertyName)
{
    const QMetaObject* meta = target->metaObject();
    int index = meta->indexOfProperty(propertyName);
    QMetaProperty property = meta->property(index);
    
    if (property.isConstant()) {
        setValue(property.read(target));
        return;
    }
    
    property.write(target, value());
    
    if (property.hasNotifySignal()) {
        QMetaMethod callback = this->metaObject()->method(this->metaObject()->indexOfMethod("onExternalValueChanged()"));
        bool c = connect(target, property.notifySignal(), this, callback);
        Q_ASSERT(c);
        Q_UNUSED(c);
    }
    
    m_trackedProperty = property;
}

void Option::setValue(const QVariant& value)
{
    if (isBlocked()) {
        m_newValue = value;
        emit valueChanged(m_newValue);
    } else {
        OptionPrivate::optionChanged(key(), value);
    }
}

void Option::setBlocked(bool blocked)
{
    if (blocked != m_blocked && blocked == false) { // unlocking
        m_blocked = blocked;
        commit();
    } else { // locking
        m_newValue = m_value;
        m_blocked = blocked;
    }
}

void Option::commit()
{
    Q_ASSERT(!isBlocked());
    
    if (m_newValue != m_value) {
        setValue(m_newValue);
    }
}

void Option::setValueImpl(const QVariant& value)
{
    if (m_value != value) {
        m_value = value;
        emit valueChanged(m_value);
    }
}

void Option::onExternalValueChanged()
{
    Q_ASSERT(sender());
    QVariant v = m_trackedProperty.read(sender());
    OptionPrivate::optionChanged(key(), v);
}

}} /* namespace Vatsinator::Core */
