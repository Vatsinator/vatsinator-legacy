#ifndef PLUGINGROUPBOXWIDGET_H
#define PLUGINGROUPBOXWIDGET_H

#include "widgets/pluginwidget.h"
#include "widgets/v8rwidgets_export.h"
#include <QtCore/QScopedPointer>
#include <QtWidgets/QGroupBox>

class QButtonGroup;

namespace Vatsinator { namespace Widgets {

/**
 * \ingroup Widgets
 * @{
 */
class V8RWIDGETS_EXPORT PluginGroupBoxWidget : public QGroupBox {
    Q_OBJECT

    Q_PROPERTY(QString selectedPlugin READ selectedPlugin WRITE setSelectedPlugin NOTIFY selectedPluginChanged)

signals:
    void selectedPluginChanged(QString pluginId);

public:
    PluginGroupBoxWidget(QWidget* parent = nullptr);

    virtual ~PluginGroupBoxWidget();

    void addPluginWidget(PluginWidget* pluginWidget);

    void setSelectedPlugin(const QString& pluginId);
    const QString& selectedPlugin() const { return m_selectedPlugin; }

private slots:
    void onPluginToggled(bool enabled);

private:
    QScopedPointer<QButtonGroup> m_buttonGroup;
    QString m_selectedPlugin;

}; /** @} */

}} /* namespace Vatsinator::Widgets */

#endif // PLUGINGROUPBOXWIDGET_H
