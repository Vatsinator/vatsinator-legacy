/*
 * option.h
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

#ifndef CORE_OPTION_H
#define CORE_OPTION_H

#include "coreexport.h"
#include <QObject>
#include <QMetaProperty>
#include <QVariant>

namespace Vatsinator { namespace Core {

class OptionPrivate;

/**
 * \ingroup Core
 * @{
 * 
 * The Option class represents a single setting that can be adjusted
 * to user's needs. The \c value property is shared amongst all instances
 * of \c Option.
 * 
 * \code{.cpp}
 * Option* option = new Option("values/opt1");
 * connect(option, &Option::valueChanged, [](const QVariant& value) { qDebug() << value; });
 * 
 * (...)
 * 
 * Option* another = new Option("values/opt1"); // notice the same key
 * another->setValue(QString("foo")); // Option::valueChanged is being
 *                                    // triggered for "option" variable as well
 * \endcode
 */
class __VtrCoreApi__ Option : public QObject {
    Q_OBJECT
    friend class OptionPrivate;
    
    /**
     * The \c key property keeps the Option key.
     */
    Q_PROPERTY(QString key READ key CONSTANT)
    
    /**
     * The \c value property keeps the Option value.
     */
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    
    /**
     * The \c blocked property specifies whether the option is currently
     * blocked or not.
     * 
     * If the option is blocked, all changes made to it (using i.e. the
     * \c setValue() method) are not signalized to all instances with
     * the same key. Instead, setting the value of the \c blocked property
     * back to \c false causes the new value to be saved. Also, if the value
     * of this property is \c true, it is possible to call the \c Option::revert()
     * method.
     * 
     * The default value of this property is \c false.
     */
    Q_PROPERTY(bool blocked READ isBlocked WRITE setBlocked)
    
signals:
    void valueChanged(const QVariant& newValue);

public:
    /**
     * Creates a new \c Option instance with the \c key provided.
     * The value is read from settings immediately.
     */
    explicit Option(const QString& key, QObject* parent = nullptr);
    
    /**
     * \copydoc Option::Option(const QString&, QObject*)
     * If the value is not yet set, it becomes \c defaultValue.
     */
    explicit Option(const QString& key, const QVariant& defaultValue, QObject* parent = nullptr);
    
    /**
     * Destroys this option.
     */
    virtual ~Option();
    
    /**
     * Tracks the given property for changes.
     * This can be useful if one wants to track for example a \c QSpinBox or
     * a \c QSlider value.
     * 
     * \code{.cpp}
     * QSpinBox* spinBox = new QSpinBox;
     * Option* option = new Option("values/spinbox_value");
     * option->track(spinBox, "value");
     * // now, whenever user changes value of the spinBox, Option will emit
     * // the valueChanged() across all instances.
     * \endcode
     */
    void track(QObject* target, const char* propertyName);
    
    const QString& key() const { return m_key; }
    const QVariant& value() const { return m_value; }
    void setValue(const QVariant& value);
    bool isBlocked() const { return m_blocked; }
    void setBlocked(bool blocked);
    
private:
    void commit();
    
private slots:
    void setValueImpl(const QVariant& value);
    void onExternalValueChanged();
    
private:
    QString m_key;
    QVariant m_value;
    QVariant m_newValue; /**< Used between blocking/unblocking */
    QMetaProperty m_trackedProperty;
    bool m_blocked = false;

}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_OPTION_H
