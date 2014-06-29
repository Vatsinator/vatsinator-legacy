/*
 * abstractanimation.h
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

#ifndef ABSTRACTANIMATION_H
#define ABSTRACTANIMATION_H

#include <QObject>

#include "vatsimdata/lonlat.h"

class AbstractAnimation : public QObject {
  
  /**
   * The base class for all map animations, like smooth move, zoom, etc.
   * Each step is calculated in protected stepEvent() method. To stop
   * the animation, use stopAnimation() function. Use position() and zoom()
   * methods to obtain current map position and zoom and use setPosition()
   * and setZoom() to set these values.
   * The default interval is 20 milliseconds. This should give enough time
   * to calculate the next step, keeping the animation smooth. To override
   * this value, use setPrefferedInterval() method.
   */
  
  Q_OBJECT

signals:
  void step();
  void stopped();
  
public:
  enum Flag {
    
    /**
     * User can't move or zoom during the animation. If he does, the animation
     * will be aborted.
     */
    OverridePositionAndZoom     = 0x0,
    
    /*
     * When set, user can zoom out & in during the animation.
     */
    NoPositionOverride  = 0x1,
    
    /**
     * When set, user can move the center point during the animation.
     */
    NoZoomOverride      = 0x2
  };
  Q_DECLARE_FLAGS(Flags, Flag);
  
  AbstractAnimation(QObject* = nullptr);
  
public slots:
  void start();
  void abort();
  
  void setPosition(const LonLat&);
  void setZoom(int);
  void setFlags(Flags);
  
  inline const LonLat& position() const { return __position; }
  inline int zoom() const { return __zoom; }
  inline Flags flags() const { return __flags; }
  inline bool isRunning() const { return __isRunning; }
  
protected:
  void stopAnimation();
  void setPrefferedInterval(int);
  
  void timerEvent(QTimerEvent*) override;
  
  virtual void stepEvent() = 0;
  
private:
  LonLat __position;
  int    __zoom;
  Flags  __flags;
  int    __interval;
  int    __timerId;
  bool   __isRunning;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractAnimation::Flags);

#endif // ABSTRACTANIMATION_H
