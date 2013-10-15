/*
    debugwindow.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui>

#include "glutils/glresourcemanager.h"
#include "glutils/vertexbufferobject.h"

#include "ui/widgets/mapwidget.h"

#include "debugging/glerrors.h"

#include "debugwindow.h"
#include "defines.h"

DebugWindow::DebugWindow(QWidget* _parent) :
    QWidget(_parent) {
  setupUi(this);
  
  connect(qApp, SIGNAL(aboutToQuit()),
          this, SLOT(hide()));
  
  __setWindowPosition();

  connect(RefreshButton,  SIGNAL(clicked()),
          this,   SLOT(__updateContents()));

  QStringList labels = {
    "Extension",
    "Pointer"
  };

  ExtensionsTable->setHorizontalHeaderLabels(labels);
  ExtensionsTable->setColumnWidth(0, 200);
  ExtensionsTable->setColumnWidth(1, 130);

}

void
DebugWindow::show() {
  OpenGLErrorsField->setPlainText(glErrors);
  GPUMemoryUsageField->setText(QString::number(gpuMemoryUsage));
  OpenGLVersionField->setText(__getGlVersion());
  VBOsCreatedField->setText(QString::number(VertexBufferObject::vboCount));
  TexturesLoadedField->setText(QString::number(GlResourceManager::textureCount()));
  NumberOfScreensField->setText(QString::number(QApplication::desktop()->screenCount()));
  IsVirtualDesktopField->setText(QApplication::desktop()->isVirtualDesktop() ? "yes" : "no");

  ExtensionsTable->clearContents();
  ExtensionsTable->setRowCount(extensions.size());
  int row = 0;

  for (auto it = extensions.begin(); it != extensions.end(); ++it) {
    QTableWidgetItem* extensionName = new QTableWidgetItem(it.key());
    QTableWidgetItem* extensionPtr = new QTableWidgetItem("0x" + QString::number(it.value(), 16));

    ExtensionsTable->setItem(row, 0, extensionName);
    ExtensionsTable->setItem(row, 1, extensionPtr);

    row += 1;
  }

  QWidget::show();
}

void
DebugWindow::__setWindowPosition() {
  QDesktopWidget* desktop = QApplication::desktop();

  int screenWidth, width;
  int screenHeight, height;

  int x, y;

  QSize windowSize;

  screenWidth = desktop -> width();
  screenHeight = desktop -> height();

  windowSize = size();
  width = windowSize.width();
  height = windowSize.height();

  x = (screenWidth - width) / 2;
  y = (screenHeight - height) / 2;
  y -= 50;

  move(x, y);
}

QString
DebugWindow::__getGlVersion() {
  QGLFormat::OpenGLVersionFlags ogvf = QGLFormat::openGLVersionFlags();

  if (ogvf & QGLFormat::OpenGL_Version_4_0)
    return "4.0";
  else if (ogvf & QGLFormat::OpenGL_Version_3_3)
    return "3.3";
  else if (ogvf & QGLFormat::OpenGL_Version_3_2)
    return "3.2";
  else if (ogvf & QGLFormat::OpenGL_Version_3_1)
    return "3.1";
  else if (ogvf & QGLFormat::OpenGL_Version_3_0)
    return "3.0";
  else if (ogvf & QGLFormat::OpenGL_Version_2_1)
    return "2.1";
  else if (ogvf & QGLFormat::OpenGL_Version_2_0)
    return "2.0";
  else if (ogvf & QGLFormat::OpenGL_Version_1_5)
    return "1.5";
  else if (ogvf & QGLFormat::OpenGL_Version_1_4)
    return "1.4";
  else if (ogvf & QGLFormat::OpenGL_Version_1_3)
    return "1.3";
  else if (ogvf & QGLFormat::OpenGL_Version_1_2)
    return "1.2";
  else if (ogvf & QGLFormat::OpenGL_Version_1_1)
    return "1.1";
  else if (ogvf & QGLFormat::OpenGL_Version_None)
    return "None";

  return "None";
}

void
DebugWindow::__updateContents() {
  OpenGLErrorsField->setPlainText(glErrors);
  GPUMemoryUsageField->setText(QString::number(gpuMemoryUsage));
  VBOsCreatedField->setText(QString::number(VertexBufferObject::vboCount));
  TexturesLoadedField->setText(QString::number(GlResourceManager::textureCount()));
}
