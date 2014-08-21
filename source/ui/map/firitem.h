/*
 * firitem.h
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

#ifndef FIRITEM_H
#define FIRITEM_H

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>

#include "ui/map/mapitem.h"

class QOpenGLShaderProgram;
class Fir;
class MapScene;

class FirItem : public QObject, public MapItem {
  Q_OBJECT
  
public:
  FirItem(const Fir*, QObject* = nullptr);
  FirItem() = delete;
  
  virtual ~FirItem();
  
  void drawBorders() const;
  void drawBackground() const;
  
  bool isVisible() const override;
  bool isLabelVisible() const override;
  const LonLat& position() const override;
  void drawItem(QOpenGLShaderProgram*) const override;
  void drawLabel(QOpenGLShaderProgram*) const override;
  void drawFocused(QOpenGLShaderProgram*) const override;
  QString tooltipText() const override;
  void showDetails() const override;
  
  inline const Fir* data() const { return __fir; }
  
private:
  void __initializeBuffers();
  void __initializeLabel() const;
  
private slots:
  void __resetLabel();
  void __invalidate();
  
private:
  MapScene*  __scene;
  const Fir* __fir;
  LonLat     __position;
  
  QOpenGLBuffer __borders;
  QOpenGLBuffer __triangles;
  mutable QOpenGLVertexArrayObject __vaoBorders;
  mutable QOpenGLVertexArrayObject __vaoTriangles;
  int __bordersVertices, __trianglesVertices;
  
  mutable QOpenGLTexture __label;
  
};

#endif // FIRITEM_H
