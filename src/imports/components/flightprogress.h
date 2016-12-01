/*
 * flightprogress.h
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

#ifndef IMPORTS_FLIGHTPROGRESS_H
#define IMPORTS_FLIGHTPROGRESS_H

#include <QQuickPaintedItem>

namespace Vatsinator { namespace Imports {

/**
 * \ingroup Imports
 * @{
 *
 * \code{.qml}
 * import org.eu.vatsinator.Components
 * \endcode
 * 
 * The \c FlightProgress item shows flight progress.
 */
class FlightProgress : public QQuickPaintedItem {
    Q_OBJECT
    
    /**
     * The \c progress property must contain value in range [0..100], where
     * 0 means the flight has not yet departed and 100 means the aircraft arrived
     * to its destination.
     *
     * \sa Vatsinator::Core::Pilot::progress.
     */
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)

    /**
     * Holds the color of the line and the airplane.
     */
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    
signals:
    void progressChanged(int progress);
    void colorChanged(const QColor& color);

public:
    /**
     * The default constructor, passes \c parent to the \c QQuickItem.
     */
    explicit FlightProgress(QQuickItem* parent = nullptr);
    
    /**
     * Destroys the \c FlightProgress instance.
     */
    virtual ~FlightProgress();
    
    /**
     * \copydoc QQuickPaintedItem::paint(QPainter*)
     */
    void paint(QPainter* painter) override;
    
    int progress() const { return m_progress; }
    void setProgress(int progress);
    
    const QColor& color() const { return m_color; }
    void setColor(const QColor& color);

private slots:
    void clearCache();
    
private:
    int m_progress;
    QColor m_color;

}; /** @} */

}} /* namespace Vatsinator::Imports */

#endif // IMPORTS_FLIGHTPROGRESS_H
