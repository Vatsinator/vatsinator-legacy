/*
 * aboutwindow.cpp
 * Copyright (C) 2016 Michal Garapich <michal@garapich.pl>
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

#include "aboutwindow.h"
#include "ui_aboutwindow.h"
#include <QtWidgets>

AboutWindow::AboutWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
    
    connect(ui->close, &QDialogButtonBox::rejected, this, &AboutWindow::close);
    
    ui->appName->setText(QCoreApplication::applicationName());
    
    QPixmap pixmap(QStringLiteral(":/icons/vatsinator.png"));
    ui->logo->setPixmap(pixmap);
    
    ui->version->setText(tr("Version %1").arg(QCoreApplication::applicationVersion()));
}

AboutWindow::~AboutWindow() {}

void AboutWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(),
                                    QDesktopWidget().screenGeometry(QApplication::activeWindow())));
}
