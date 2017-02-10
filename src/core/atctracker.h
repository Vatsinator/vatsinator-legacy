/*
 * atctracker.h
 * Copyright (C) 2017  Michał Garapich <michal@garapich.pl>
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

#ifndef ATCTRACKER_H
#define ATCTRACKER_H

#include "atc.h"
#include "coreexport.h"
#include <QObject>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 */
class __VtrCoreApi__ AtcTracker : public QObject {
    Q_OBJECT

    Q_PROPERTY(Vatsinator::Core::Atc* atc READ atc CONSTANT)

public:
    explicit AtcTracker(Atc* atc, QObject* parent = nullptr);

    Atc* atc() { return m_atc; }
    const Atc* atc() const { return m_atc; }

private:
    void initialize();

private:
    Atc* m_atc;

}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // ATCTRACKER_H
