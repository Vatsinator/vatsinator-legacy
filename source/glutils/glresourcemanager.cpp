/*
 * glresourcemanager.cpp
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#include <QGLWidget>

#include "ui/userinterface.h"
#include "vatsinatorapplication.h"

#include "glresourcemanager.h"

GLuint
GlResourceManager::loadImage(const QImage& _img) {
  GLuint pix;
  QImage final = QGLWidget::convertToGLFormat(_img);
  
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &pix);
  
  glBindTexture(GL_TEXTURE_2D, pix);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, final.width(), final.height(),
               0, GL_RGBA, GL_UNSIGNED_BYTE, final.bits());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR causes the text
                                                                    // on the tooltip is blurred
  
  glBindTexture(GL_TEXTURE_2D, 0);
  
  return pix;
}

GLuint
GlResourceManager::loadImage(const QString& _fName) {
  GLuint pix;
  QImage final, temp;
  
  VatsinatorApplication::log("GlResourceManager: loading texture: %s...", _fName.toStdString().c_str());
  
  if (!temp.load(_fName)) {
    vApp()->userInterface()->warning(
      QString("Image %1 could not be loaded").arg(_fName));
  }
  
  final = QGLWidget::convertToGLFormat(temp);
  
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &pix);
  
  glBindTexture(GL_TEXTURE_2D, pix);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, final.width(), final.height(),
               0, GL_RGBA, GL_UNSIGNED_BYTE, final.bits());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  glBindTexture(GL_TEXTURE_2D, 0);
  
  return pix;
}

void
GlResourceManager::deleteImage(GLuint _img) {
  glDeleteTextures(1, &_img);
}
