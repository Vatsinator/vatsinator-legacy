/*
 * abstractanimation.cpp
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

#include <QtCore>

#include "abstractanimation.h"
#include "defines.h"

AbstractAnimation::AbstractAnimation(QObject* _parent) :
    QObject(_parent),
    __position(),
    __zoom(0),
    __interval(20),
    __isRunning(false) {}

void
AbstractAnimation::start() {
  __isRunning = true;
  __timerId = startTimer(__interval);
}

void
AbstractAnimation::abort() {
  killTimer(__timerId);
}

void
AbstractAnimation::setPosition(const LonLat& _p) {
  __position = _p;
}

void
AbstractAnimation::setZoom(int _z) {
  __zoom = _z;
}

void
AbstractAnimation::setFlags(AbstractAnimation::Flags _f) {
  __flags = _f;
}

void
AbstractAnimation::stopAnimation() {
  abort();
  __isRunning = false;
}

void
AbstractAnimation::setPrefferedInterval(int _i) {
  __interval = _i;
}

void
AbstractAnimation::timerEvent(QTimerEvent* _event) {
  Q_ASSERT(_event->timerId() == __timerId);
  
  stepEvent();
  emit step();
  
  if (!__isRunning)
    emit stopped();
}
