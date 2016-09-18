/*
 * settingswindow.cpp
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "core/option.h"
#include "config.h"
#include <QtWidgets>
#include <functional>

using namespace Vatsinator::Core;

SettingsWindow::SettingsWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    fillLanguages();
    
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingsWindow::handleButton);
    
    Option* statistics = new Option("misc/statistics", this);
    statistics->track(ui->statistics, "checked");
    
    Option* language = new Option("misc/language", this);
    /* Track the QComboBox's currentData property, but there is no notifier signal for it */
    connect(ui->language, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [language, this](int index) {
        QString locale = this->ui->language->itemData(index).toString();
        language->setValue(locale);
    });

    QString locale = language->value().toString();
    int index = ui->language->findData(locale);
    ui->language->setCurrentIndex(index);
    
    m_options << statistics << language;
    
    // doesn't work on Linux, I know
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
}

SettingsWindow::~SettingsWindow() {}

void SettingsWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(),
                                    QDesktopWidget().screenGeometry(QApplication::activeWindow())));
    
#ifndef Q_OS_MAC
    std::for_each(m_options.begin(), m_options.end(), std::bind(&Option::setBlocked, std::placeholders::_1, true));
#endif
}

void SettingsWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        event->accept();
    } else {
        event->ignore();
    }
}

void SettingsWindow::fillLanguages()
{
    QDir dir(QString(VATSINATOR_PREFIX "translations"));
    QStringList trs = dir.entryList({ "vatsinator_*.qm" });
    for (auto tr: trs) {
        QString locale = tr;
        locale.truncate(locale.lastIndexOf('.'));
        locale.remove(0, locale.indexOf('_') + 1);
        
        QString lang = QLocale::languageToString(QLocale(locale).language());
        ui->language->addItem(lang, locale);
    }
}

void SettingsWindow::handleButton(QAbstractButton* button)
{
    if (ui->buttonBox->button(QDialogButtonBox::Apply) == button) {
        // TODO Use Option::commit() someday maybe?
        std::for_each(m_options.begin(), m_options.end(), std::bind(&Option::setBlocked, std::placeholders::_1, false));
        std::for_each(m_options.begin(), m_options.end(), std::bind(&Option::setBlocked, std::placeholders::_1, true));
    } else if (ui->buttonBox->button(QDialogButtonBox::Ok) == button) {
        std::for_each(m_options.begin(), m_options.end(), std::bind(&Option::setBlocked, std::placeholders::_1, false));
        close();
    } else if (ui->buttonBox->button(QDialogButtonBox::Cancel) == button) {
        // TODO Revert
        close();
    }
}
