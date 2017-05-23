#ifndef WIDGETS_PLUGINWIDGET_H
#define WIDGETS_PLUGINWIDGET_H

#include "widgets/vtrwidgets_export.h"
#include <QWidget>

class QRadioButton;
namespace Ui { class PluginWidget; }

namespace Vatsinator { namespace Widgets {

class PluginGroupBoxWidget;

/**
 * \ingroup Widgets
 * @{
 *
 * PluginWidget represents a single plugin in the UI.
 *
 * The PluginWidget can be used to display a single plugin basic information and
 * let user enable or disable it.
 *
 * \sa PluginGroupBoxWidget.
 */
class VTRWIDGETS_EXPORT PluginWidget : public QWidget {
    Q_OBJECT

    /**
     * The \c pluginId property identifies the plugin in a unique manner.
     */
    Q_PROPERTY(QString pluginId READ pluginId)

    /**
     * This property holds the plugin name, so that user can recognize it.
     */
    Q_PROPERTY(QString pluginName READ pluginName WRITE setPluginName)

    /**
     * The \c pluginDescription property is used to describe the plugin briefly.
     */
    Q_PROPERTY(QString pluginDescription READ pluginDescription WRITE setPluginDescription)

    /**
     * Specifies whether the plugin is currently enabled or not.
     */
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY toggled)

    /**
     * This property specifies whether this plugin is one of many from which
     * only one can be active at a time.
     * This property is automatically set to \c true by the \ref PluginGroupBoxWidget.
     */
    Q_PROPERTY(bool exclusive READ isExclusive WRITE setExclusive)

signals:
    void toggled(bool enabled);

public:
    /**
     * Creates a new \c PluginWidget instance with the given \c pluginId.
     * The \c pluginId should be unique.
     */
    explicit PluginWidget(const QString& pluginId, QWidget *parent = nullptr);
    ~PluginWidget();

    QString pluginName() const;
    void setPluginName(const QString& pluginName);

    QString pluginDescription() const;
    void setPluginDescription(const QString& pluginDescription);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool isExclusive() const;
    void setExclusive(bool exclusive);

    QString pluginId() const;

private:
    friend class PluginGroupBoxWidget;

    QRadioButton* radioButton();

    QScopedPointer<Ui::PluginWidget> ui;
    QString m_pluginId;
    QString m_pluginName;
    QString m_pluginDescription;
    bool m_enabled;
    bool m_exclusive;

}; /** @} */

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_PLUGINWIDGET_H
