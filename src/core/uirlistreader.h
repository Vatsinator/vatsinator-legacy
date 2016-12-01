/*
 * uirlistreader.h
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

#ifndef CORE_UIRLISTREADER_H
#define CORE_UIRLISTREADER_H

#include "coreexport.h"
#include "core/resourcefile.h"
#include <QMap>
#include <tuple>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 *
 * A reader for the Upper FIR list.
 */
class __VtrCoreApi__ UirListReader : public ResourceFile {
    Q_OBJECT

public:
    using UirData = std::tuple<QString, QString, QList<QString>>; /**< ICAO, name and list of FIRs's ICAO codes */

    explicit UirListReader(QObject* parent = nullptr);

    std::pair<bool, UirData> find(const QString& icao);

private slots:
    void readData();

private:
    QMap<QString, UirData> m_uirs;

}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_UIRLISTREADER_H
