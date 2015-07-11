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

/**
 * An utility class that makes it possible to access native parts
 * of Android's from the C++ code.
 */
class AndroidBridge : public QObject {
    Q_OBJECT
    
    /**
     * Defines the navigation bar color.
     */
    Q_PROPERTY(QColor navigationBarColor
        READ navigationBarColor
        WRITE setNavigationBarColor
        RESET resetNavigationBarColor
        NOTIFY navigatinBarColorChanged
    )
    
    /**
     * Defines the status bar color.
     */
    Q_PROPERTY(QColor statusBarColor
        READ statusBarColor
        WRITE setStatusBarColor
        NOTIFY statusBarColorChanged
    )
    
signals:
    void navigatinBarColorChanged(const QColor& navigationBarColor);
    void statusBarColorChanged(const QColor& statusBarColor);

public:
    explicit AndroidBridge(QObject* parent = nullptr);
    
    void setNavigationBarColor(const QColor& color);
    void resetNavigationBarColor();
    
    void setStatusBarColor(const QColor& color);
    
    inline const QColor& navigationBarColor() const
    {
        return __navigationBarColor;
    }
    
    inline const QColor& statusBarColor() const
    {
        return __statusBarColor;
    }
    
private slots:
    void __setNavigationBarColorImpl(const QColor& navigationBarColor);
    void __setStatusBarColorImpl(const QColor& statusBarColor);
    
private:
    QColor __navigationBarColor;
    QColor __statusBarColor;
    
};

#endif // ANDROIDBRIDGE_H
