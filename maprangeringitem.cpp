#include "maprangeringitem.h"
#include "graphicsmap.h"
#include <QPainter>

MapRangeRingItem::MapRangeRingItem() :
    m_radius(60)
{
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
}

void MapRangeRingItem::setCoord(const QGeoCoordinate &coord)
{
    if(m_coord == coord)
        return;
    m_coord = coord;
    m_center = GraphicsMap::toScene(m_coord);
    setPos(m_center);
    updateBoundingRect();
    update();
}

void MapRangeRingItem::setRadius(const float &km)
{
    if(m_radius == km)
        return;
    m_radius = km;
    updateBoundingRect();
    update();
}

void MapRangeRingItem::setPen(const QPen &pen)
{
    if(m_pen == pen)
        return;
    m_pen = pen;
    update();
}

void MapRangeRingItem::setFont(const QFont &font)
{
    if(m_font == font)
        return;
    m_font = font;
    update();
}

QRectF MapRangeRingItem::boundingRect() const
{
    return m_boundRect;
}

void MapRangeRingItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    painter->setPen(m_pen);
    painter->setFont(m_font);
    drawEllipse(painter, m_radius);
}

void MapRangeRingItem::updateBoundingRect()
{
    prepareGeometryChange();
    qreal halfpw = m_pen.style() == Qt::NoPen ? qreal(0) : m_pen.widthF() / 2;
    auto topCoord = m_coord.atDistanceAndAzimuth(m_radius * 1e3, 0);
    auto rightCoord = m_coord.atDistanceAndAzimuth(m_radius * 1e3, 90);
    auto top = GraphicsMap::toScene(topCoord);
    auto right = GraphicsMap::toScene(rightCoord);
    auto dx =  right.rx() - m_center.x();
    auto dy = m_center.y() - top.ry();
    m_boundRect.adjust(-dx, -dy, dx, dy);
    m_boundRect.adjust(-halfpw, -halfpw, halfpw, halfpw);
}

void MapRangeRingItem::drawEllipse(QPainter *painter, const float &radius)
{
    auto topCoord = m_coord.atDistanceAndAzimuth(radius * 1e3, 0);
    auto rightCoord = m_coord.atDistanceAndAzimuth(radius * 1e3, 90);
    auto top = GraphicsMap::toScene(topCoord);
    auto right = GraphicsMap::toScene(rightCoord);
    painter->drawEllipse({0, 0}, right.rx()-m_center.rx(), m_center.ry()-top.ry());
}
