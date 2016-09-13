/*
 * metar.h
 * Copyright (C) 2012  Michał Garapich <michal@garapich.pl>
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

#ifndef CORE_METAR_H
#define CORE_METAR_H

#include "coreexport.h"
#include <QMetaType>
#include <QSharedData>
#include <QString>

namespace Vatsinator { namespace Core {

class MetarData;

/**
 * \ingroup Core
 * @{
 *
 * The Metar class encapsulates a single METAR report.
 */
class __VtrCoreApi__ Metar {

public:
    Metar();
    explicit Metar(const QString& metar);
    Metar(const Metar& other);
    virtual ~Metar();
    Metar& operator=(const Metar& other);
    
    QString icao() const;
    void setIcao(const QString& icao);
    QString metar() const;
    void setMetar(const QString& metar);
    
private:
    QSharedDataPointer<MetarData> d;

}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::Metar)
Q_DECLARE_TYPEINFO(Vatsinator::Core::Metar, Q_MOVABLE_TYPE);

#endif // CORE_METAR_H
