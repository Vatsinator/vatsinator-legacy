/*
 * androidbridge.h
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

#ifndef IMPORTS_ANDROIDBRIDGE_H
#define IMPORTS_ANDROIDBRIDGE_H

#include <QObject>

namespace Vatsinator { namespace Imports {

/**
 * \ingroup Imports
 * @{
 * 
 * \code{.qml}
 * import org.eu.vatsinator.NativeUtils
 * \endcode
 * 
 * A couple of utilities that make it easy to retrieve info about currently
 * running Android system.
 */
class AndroidBridge : public QObject {
    Q_OBJECT

    /**
     * Height, in pixels, of the status bar.
     */
    Q_PROPERTY(int statusBarHeight READ statusBarHeight CONSTANT)
    
    /**
     * Height, in pixels, of the navigation bar.
     * When in landscape mode, it is always 0.
     */
    Q_PROPERTY(int navigationBarHeight
        READ navigationBarHeight
        NOTIFY navigationBarHeightChanged)
    
signals:
    void navigationBarHeightChanged(int navigationBarHeight);

public:
    explicit AndroidBridge(QObject* parent = nullptr);
    
    int statusBarHeight() const { return m_statusBarHeight; }
    int navigationBarHeight() const { return m_navigationBarHeight; }
    
private:
    void fetchStatusBarHeight();
    void fetchNavigationBarHeight();
    
private slots:
    void onScreenOrientationChanged();
    
private:
    int m_statusBarHeight;
    int m_navigationBarHeight = 0;
    
}; /** @} */

}} /* namespace Vatsinator::Imports */

#endif // IMPORTS_ANDROIDBRIDGE_H
