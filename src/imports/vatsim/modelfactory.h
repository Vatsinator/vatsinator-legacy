/*
 * modelfactory.h
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

#ifndef IMPORTS_MODELFACTORY_H
#define IMPORTS_MODELFACTORY_H

#include "misc/atclistmodel.h"
#include "misc/clientlistmodel.h"
#include "misc/flightlistmodel.h"
#include "misc/notamlistmodel.h"
#include <QObject>

namespace Vatsinator { namespace Imports {

/**
 * \ingroup Imports
 * @{
 * 
 * The ModelFactory class is an utility class that makes it possible
 * to insantiate item models from QML code.
 */
class ModelFactory : public QObject {
    Q_OBJECT

public:
    explicit ModelFactory(QObject* parent = nullptr);
    virtual ~ModelFactory();
    
    /**
     * \sa Vatsinator::Misc::ClientListModel::clients(Vatsinator::Core::AirportObject*),
     *  Vatsinator::Misc::ClientListModel::clients(Vatsinator::Core::FirObject*)
     */
    Q_INVOKABLE Vatsinator::Misc::ClientListModel* clients(QVariant object) const;
    
    /**
     * Creates a new NotamListModel for the provided object.
     * \c object's type can be either \c AirportObject* or \c FirObject*.
     */
    Q_INVOKABLE Vatsinator::Misc::NotamListModel* notams(QVariant object);
    
}; /** @} */

}} /* namespace Vatsinator::Imports */

#endif // IMPORTS_MODELFACTORY_H
