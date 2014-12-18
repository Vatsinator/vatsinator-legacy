/*
 * vatsimdatadocument.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#ifndef VATSIMDATADOCUMENT_H
#define VATSIMDATADOCUMENT_H

#include <QObject>

/**
 * The VatsimDataDocument class represents a single data file that is
 * being fetched periodically from the internet. It contains list of connected
 * clients and preflights.
 */
class VatsimDataDocument : public QObject {
    Q_OBJECT

private:
};

#endif // VATSIMDATADOCUMENT_H
