/*
 * metarwindow.cpp
 * Copyright (C) 2012  Michał Garapich <michal@garapich.pl>
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

#include "metarwindow.h"
#include "ui_metarwindow.h"
#include "widgetsprivate.h"
#include <QtCore>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Widgets {

MetarWindow::MetarWindow(ServerTracker *server, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetarWindow)
{
    ui->setupUi(this);

    connect(ui->metarSearch, &QLineEdit::textChanged, this, &MetarWindow::queryChanged);

    QSortFilterProxyModel* model = new QSortFilterProxyModel(this);
    ui->metars->setModel(model);
    model->setFilterCaseSensitivity(Qt::CaseInsensitive);

    connect(server, &ServerTracker::metarModelChanged, model, &QSortFilterProxyModel::setSourceModel);
    model->setSourceModel(server->metars());
}

MetarWindow::~MetarWindow() {}

void MetarWindow::show(const QString& icaoSearch)
{
    QWidget::show();
    ui->metarSearch->setText(icaoSearch.toUpper());
}

void MetarWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    fixupGeometry(this);
}

void MetarWindow::queryChanged()
{
    QSortFilterProxyModel* model = qobject_cast<QSortFilterProxyModel*>(ui->metars->model());
    Q_ASSERT(model);

    if (ui->metarSearch->text().isEmpty()) {
        model->setFilterRegExp(QString());
    } else {
        QString regex = QStringLiteral("^%1").arg(ui->metarSearch->text());
        model->setFilterRegExp(regex);
    }
}

}} /* namespace Vatsinator::Widgets */
