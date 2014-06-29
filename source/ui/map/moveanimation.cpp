/*
 * moveanimation.cpp
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

#include "moveanimation.h"
#include "defines.h"

MoveAnimation::MoveAnimation(QObject* _parent): AbstractAnimation(_parent) {
  setFlags(NoZoomOverride);
}

void
MoveAnimation::setTarget(const LonLat& _t) {
  __target = _t;
}

void MoveAnimation::stepEvent() {
  LonLat p = position();
  qreal vecX = __target.x() - p.x();
  qreal vecY = __target.y() - p.y();
  qreal len = sqrt(vecX * vecX + vecY * vecY) * 2;
  
  if (len > 1.0) {
    /* Normalize */
    vecX = vecX / len;
    vecY = vecY / len;
  
    p.rx() += vecX;
    p.ry() += vecY;
  } else {
    p = __target;
  }
  
  setPosition(p);
  
  if (p == target())
    stopAnimation();
}
