/*
 * dataupdater.cpp
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

#include <QtNetwork>

#include "network/filedownloader.h"

#include "storage/filemanager.h"
#include "storage/unzipper.h"

#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "dataupdater.h"

static const QString PackageUrl =
    NetConfig::Vatsinator::repoUrl() % QStringLiteral("packages/latest.zip");
static const QString ManifestUrl =
    NetConfig::Vatsinator::repoUrl() % QStringLiteral("packages/Manifest");

namespace {

/**
 * Moves the file.
 */
bool moveFile(const QString& oldLocation, const QString& newLocation)
{
    QFile file(oldLocation);
    
    if (!file.open(QIODevice::ReadWrite)) {
        qCritical("DataUpdater: failed accessing file %s", qPrintable(oldLocation));
        return false;
    }
    
    file.close();
    
    QFile oldFile(newLocation);
    
    if (oldFile.exists())
        oldFile.remove();
        
    QFileInfo fileInfo = QFileInfo(newLocation);
    
    if (!QDir(fileInfo.path()).exists())
        QDir().mkpath(fileInfo.path());
        
    bool result = file.rename(newLocation);
    
    if (result)
        qDebug("DataUpdater: moved file %s -> %s", qPrintable(oldLocation), qPrintable(newLocation));
    else
        qCritical("DataUpdater: failed moving %s -> %s", qPrintable(oldLocation), qPrintable(newLocation));
        
    return result;
}

/**
 * Removes the dir recursively.
 * http://stackoverflow.com/questions/11050977/removing-a-non-empty-folder-in-qt
 */
bool removeDir(const QString& dirName)
{
    bool result = true;
    QDir dir(dirName);
    
    if (dir.exists()) {
        for (QFileInfo file : dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (file.isDir()) {
                qDebug("DataUpdater: removing dir %s", qPrintable(file.absoluteFilePath()));
                result = removeDir(file.absoluteFilePath());
            } else {
                qDebug("DataUpdater: removing file %s", qPrintable(file.absoluteFilePath()));
                result = QFile::remove(file.absoluteFilePath());
            }
            
            if (!result)
                return result;
        }
        
        result = dir.rmdir(dirName);
    }
    
    return result;
}

bool checksumMatches(const QString& fileName, const QByteArray& md5)
{
    QFile file(fileName);
    
    if (!file.open(QIODevice::ReadOnly))
        return false;
        
    auto hash = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
    file.close();
    
    return hash == md5;
}

}

DataUpdater::DataUpdater(QObject* parent) :
    QObject(parent),
    __unzipper(new Unzipper())
{

    /* Give the Unzipper its own thread */
    QThread* thread = new QThread();
    __unzipper->moveToThread(thread);
    connect(__unzipper, &Unzipper::unzipped, this, &DataUpdater::__filesUnzipped);
    connect(__unzipper, &Unzipper::error, this, &DataUpdater::__unzipError);
    connect(this, &DataUpdater::readyToUnzip, __unzipper, &Unzipper::unzip);
    thread->start();
}

DataUpdater::~DataUpdater()
{
    QThread* thread = __unzipper->thread();
    __unzipper->deleteLater();
    thread->quit();
    thread->wait();
    delete thread;
}

void
DataUpdater::update()
{
    qDebug("DataUpdater: updating...");
    
    FileDownloader* fd = new FileDownloader();    
    connect(fd, &FileDownloader::finished, this, &DataUpdater::__unzipPackage);
    connect(fd, &FileDownloader::error, this, &DataUpdater::__fetchError);
    fd->fetch(QUrl(PackageUrl));
}

bool
DataUpdater::__checksumsOk(const QString& fileName)
{
    QFile file(fileName);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("DataUpdater: cannot open %s for reading", qPrintable(fileName));
        return false;
    }
    
    QHash<QString, QString> md5sums;
    
    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine()).simplified();
        
        if (line.isEmpty())
            continue;
            
        if (QRegExp("\\d{8}").exactMatch(line)) { // manifest date
            qDebug("DataUpdater: manifest date: %s", qPrintable(line));
            continue;
        }
        
        QStringList lineSplitted = line.split(QRegExp("\\s+"));
        Q_ASSERT(lineSplitted.length() == 2);
        
        lineSplitted[1].replace(QString("./"), QString(""));
        
        // insert filename <-> md5sum pair
        md5sums.insert(lineSplitted[1], lineSplitted[0]);
    }
    
    file.close();
    
    for (const QString& f : __unzipper->fileList()) {
        if (!md5sums.contains(f)) {
            qCritical("DataUpdater: could not find the md5 sum for file %s!", qPrintable(f));
            return false;
        }
        
        if (!checksumMatches(__unzipper->targetDir() + f, md5sums[f].toUtf8())) {
            qCritical("DataUpdater: checksum failed for %s", qPrintable(f));
            return false;
        }
    }
    
    return true;
}

bool
DataUpdater::__moveFiles()
{
    for (const QString& file : __unzipper->fileList()) {
        if (!moveFile(__unzipper->targetDir() % file, FileManager::localDataPath() % file))
            return false;
    }
    
    return true;
}

void
DataUpdater::__cleanup()
{
    removeDir(__unzipper->targetDir());
    
    if (QFile(__unzipper->fileName()).exists())
        QFile::remove(__unzipper->fileName());
}

void
DataUpdater::__unzipPackage(QString fileName)
{
    __unzipper->setFileName(fileName);
    emit readyToUnzip();
    
    FileDownloader* d = qobject_cast<FileDownloader*>(sender());
    if (d)
        d->deleteLater();
}

void
DataUpdater::__fetchError(QString error)
{
    qWarning("DataUpdater: fetch error: %s", qPrintable(error));
    emit failed();
    
    FileDownloader* d = qobject_cast<FileDownloader*>(sender());
    if (d)
        d->deleteLater();
}

void
DataUpdater::__filesUnzipped()
{
    qDebug("DataUpdater: files unzipped.");
    
    FileDownloader* fd = new FileDownloader();
    connect(fd, &FileDownloader::finished, this, &DataUpdater::__checkManifest);
    connect(fd, &FileDownloader::error, this, &DataUpdater::__fetchError);
    fd->fetch(QUrl(ManifestUrl));
}

void
DataUpdater::__unzipError(QString error)
{
    qCritical("DataUpdater: unzip error: %s", qPrintable(error));
    emit failed();
}

void
DataUpdater::__checkManifest(QString fileName)
{
    FileDownloader* d = qobject_cast<FileDownloader*>(sender());
    if (d)
        d->deleteLater();
    
    if (!__checksumsOk(fileName)) {
        __cleanup();
        emit failed();
        return;
    }
    
    if (!__moveFiles()) {
        __cleanup();
        emit failed();
        return;
    }
    
    moveFile(fileName, FileManager::localDataPath() % "Manifest");
    __cleanup();
    emit updated();
}