/*
 * delayedwidget.h
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

#ifndef DELAYEDWIDGET_H
#define DELAYEDWIDGET_H

#include <QWidget>

class QMovie;

/**
 * The DelayedWidget simplifies creation of a widget which contents may
 * be loaded after its first show event.
 * There are two states in which the widget can be - Loading and Finished.
 * During the first state, the specified animation or text is being shown.
 * When subclassing, the DelayedWidget's paintEvent() implementation must
 * be called unless the state is Finished.
 * 
 * void MyWidget::paintEvent(QPaintEvent* event) {
 *   if (status() == DelayedWidget::Loading) {
 *      DelayedWidget::paintEvent(event);
 *   } else { ... }
 * }
 * 
 * DelayedWidget::Loading is the default status.
 */
class DelayedWidget : public QWidget {
  Q_OBJECT
  Q_ENUMS(Status)
  
  /**
   * Describes the status of the widget.
   */
  Q_PROPERTY(Status status READ status WRITE setStatus)
  
  /**
   * Keeps the current animation.
   * The animation can be shown only when status == Loading.
   */
  Q_PROPERTY(QMovie* animation READ animation WRITE setAnimation)

public:
  enum Status { Loading, Finished };
  
  /**
   * The status is set to Loading by default.
   */
  explicit DelayedWidget(QWidget* = 0, Qt::WindowFlags = 0);
  
  void setStatus(Status);
  void setAnimation(QMovie*);
  
  inline Status status() const { return __status; }
  inline const QMovie* animation() const { return __animation; }
  inline QMovie* animation() { return __animation; }
  
protected:
  virtual void paintEvent(QPaintEvent*) override;
  
private:
  Status __status;
  QMovie* __animation;
  
};

#endif // DELAYEDWIDGET_H
