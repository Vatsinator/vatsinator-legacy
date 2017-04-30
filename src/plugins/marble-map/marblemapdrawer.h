#ifndef MARBLEMAPDRAWER_H
#define MARBLEMAPDRAWER_H

#include "core/lonlat.h"
#include "gui/mapdrawer.h"
#include "gui/maprenderer.h"
#include <QObject>

namespace Marble { class MarbleMap; }

namespace MarbleMap {

class MarbleMapDrawer : public QObject, public Vatsinator::Gui::MapDrawer {
    Q_OBJECT

public:
    explicit MarbleMapDrawer(Vatsinator::Gui::MapRenderer* renderer, QObject *parent = nullptr);
    virtual ~MarbleMapDrawer();

    void draw(Vatsinator::Gui::WorldPainter* painter) override;

private slots:
    void setMarbleCenter(const Vatsinator::Core::LonLat& center);
    void setMarbleSize(const QSize& size);

private:
    QScopedPointer<Marble::MarbleMap> m_marbleMap;

};

} /* namespace MarbleMapDrawer */

#endif // MARBLEMAPDRAWER_H
