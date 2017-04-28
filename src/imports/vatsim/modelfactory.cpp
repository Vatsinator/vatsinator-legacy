/*
 * modelfactory.cpp
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

#include "modelfactory.h"
#include "core/pluginfinder.h"
#include "misc/notamprovider.h"

using namespace Vatsinator::Core;
using namespace Vatsinator::Misc;

namespace Vatsinator { namespace Imports {

ModelFactory::ModelFactory(QObject* parent) : QObject(parent) {}

ModelFactory::~ModelFactory() {}

ClientListModel* ModelFactory::clients(QVariant object) const
{
    ClientListModel* model = nullptr;
    
    if (object.canConvert<AirportObject*>()) {
        model = ClientListModel::clients(object.value<AirportObject*>());
    } else if (object.canConvert<FirObject*>()) {
        model = ClientListModel::clients(object.value<FirObject*>());
    }
    
    if (model)
        model->setSorted(true);
    
    return model;
}

NotamListModel* ModelFactory::notams(QVariant object)
{
    NotamProvider* provider = nullptr;
    
    auto plugins = PluginFinder::pluginsForIid(qobject_interface_iid<NotamProvider*>());
    if (plugins.length() > 0) {
        provider = qobject_cast<NotamProvider*>(PluginFinder::plugin(plugins.first()));
        Q_ASSERT(provider);
    } else {
        return nullptr;
    }
    
    QString icao;
    if (object.canConvert<AirportObject*>())
        icao = object.value<AirportObject*>()->icao();
    else if (object.canConvert<FirObject*>())
        icao = object.value<FirObject*>()->icao();
    else
        return nullptr;
    
    const NotamReply* nr = provider->fetchNotams(icao);
    
    if (nr->isFinished())
        return new NotamListModel(nr->notams(), this);
    else
        return new NotamListModel(this);
}

}} /* namespace Vatsinator::Imports */
