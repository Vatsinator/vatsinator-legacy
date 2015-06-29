/*
 * paletteprovider.h
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

#ifndef PALETTEPROVIDER_H
#define PALETTEPROVIDER_H

#include <QObject>
#include <QColor>

/**
 * The PaletteProvider class is an easy way to get the color
 * that fits the whole app color palette.
 * 
 * See http://www.google.com/design/spec/style/color.html#color-color-palette
 * to get the idea of how it works.
 * 
 * The palette chosen for Vatsinator app is "Light Blue", accent
 * palette is "Red".
 */
class PaletteProvider : public QObject {
    Q_OBJECT

public:
    explicit PaletteProvider(QObject *parent = nullptr);
    
    /**
     * Gets the color from the palette by hue.
     * Use +1000 to get the accent color.
     */
    Q_INVOKABLE QColor byHue(int hue) const;
    
    /**
     * Gets the background color, which is, pretty much, a white one.
     */
    Q_INVOKABLE QColor background() const;
    
    /**
     * Color of dividers, i.e. lines that split list view elements.
     */
    Q_INVOKABLE QColor divider() const;
};

#endif // PALETTEPROVIDER_H
