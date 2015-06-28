/*
 * androidbridge.h
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

#ifndef ANDROIDBRIDGE_H
#define ANDROIDBRIDGE_H

#include <QObject>
#include <QColor>

class AndroidBridge : public QObject {
    Q_OBJECT
    
    /**
     * Defines the navigation bar color in Android.
     */
    Q_PROPERTY(QColor navigationBarColor READ navigationBarColor WRITE setNavigationBarColor RESET resetNavigationBarColor NOTIFY navigatinBarColorChanged)
    
signals:
    void navigatinBarColorChanged(const QColor& navigationBarColor);

public:
    explicit AndroidBridge(QObject* parent = nullptr);
    
    void setNavigationBarColor(const QColor& color);
    void resetNavigationBarColor();
    
    inline const QColor& navigationBarColor() const
    {
        return __navigationBarColor;
    }
    
private slots:
    void __setNavigationBarColorImpl(const QColor& navigationBarColor);
    
private:
    QColor __navigationBarColor;
    
    
};

#endif // ANDROIDBRIDGE_H
