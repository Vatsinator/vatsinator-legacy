/*
 * modelmatcher.h
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#ifndef GUI_MODELMATCHER_H
#define GUI_MODELMATCHER_H

#include "guiexport.h"
#include "core/resourcefile.h"
#include <QObject>
#include <QJsonDocument>
#include <QMap>
#include <QMetaType>
#include <QString>

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 * 
 * The ModelMatcher class handles matching aircrafts filed in the flight plan
 * to the actual aircraft models.
 */
class __VtrGuiApi__ ModelMatcher : public QObject {
    Q_OBJECT

    /**
     * The file this \c ModelMatcher reads its mappings from.
     * The \c ModelMatcher takes ownership over assigned \c ResourceFile
     * instances.
     */
    Q_PROPERTY(Vatsinator::Core::ResourceFile* resourceFile READ resourceFile
               WRITE setResourceFile NOTIFY resourceFileChanged)

signals:
    void resourceFileChanged(Core::ResourceFile* resourceFile);

public:
    /**
     * Creates a new ModelMatcher instance.
     */
    explicit ModelMatcher(QObject* parent = nullptr);
    
    /**
     * Destroys the ModelMatcher instance.
     */
    virtual ~ModelMatcher();

    Core::ResourceFile* resourceFile() { return m_resourceFile; }
    const Core::ResourceFile* resourceFile() const { return m_resourceFile; }
    void setResourceFile(Core::ResourceFile* resourceFile);
    
    /**
     * Matches the given model. If no model could be matched, returns
     * \c defaultModel().
     * 
     * \param modelString The aircraft that is filled in the flight plan.
     */
    virtual QString match(const QString& modelString) const;
    
    /**
     * Returns the default model that is used as a fallback.
     */
    virtual QString defaultModel() const;

private:
    /**
     * Reads model mappings from the JSON file. Old mappings are not removed.
     */
    void readFromJson(const QJsonDocument& json);

private slots:
    void readMappingFile();
    
private:
    Core::ResourceFile* m_resourceFile = nullptr;
    QMap<QString, QString> m_mappings;
    
}; /** @} */

}} /* namespace Vatsinator::Gui */

Q_DECLARE_METATYPE(Vatsinator::Gui::ModelMatcher*)

#endif // GUI_MODELMATCHER_H
