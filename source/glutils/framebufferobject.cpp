/*
 * framebufferobject.cpp
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

#include "glutils/glextensions.h"
#include "debugging/glerrors.h"

#include "framebufferobject.h"

FrameBufferObject::FrameBufferObject(int _width, int _height) : 
     __fboId(0),
     __textureId(0),
     __rboId(0) {
  
  glGenTextures(1, &__textureId); checkGLErrors(HERE);
  glBindTexture(GL_TEXTURE_2D, __textureId); checkGLErrors(HERE);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  checkGLErrors(HERE);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  glGenFramebuffers(1, &__fboId); checkGLErrors(HERE);
  glBindFramebuffer(GL_FRAMEBUFFER, __fboId); checkGLErrors(HERE);
  
  glGenRenderbuffers(1, &__rboId); checkGLErrors(HERE);
  glBindRenderbuffer(GL_RENDERBUFFER, __rboId); checkGLErrors(HERE);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, __textureId, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, __rboId);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, __fboId);
}

void FrameBufferObject::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
