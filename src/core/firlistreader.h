/*
 * firlistreader.h
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

#ifndef CORE_FIRLISTREADER_H
#define CORE_FIRLISTREADER_H

#include "core/fir.h"
#include "core/resourcefile.h"
#include "core/vtrcore_export.h"

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 *
 * The reader for FIR list.
 */
class VTRCORE_EXPORT FirListReader : public ResourceFile {
    Q_OBJECT

public:
    /**
     * Creates a new FIR list reader. Passes \c parent to the
     * QObject's constructor.
     */
    explicit FirListReader(QObject* parent = nullptr);
    
    /**
     * Returns list of all read FIRs.
     */
    const QList<Fir>& firs() const { return m_firs; }

private slots:
    void readData();
    
private:
    QList<Fir> m_firs;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_FIRLISTREADER_H
