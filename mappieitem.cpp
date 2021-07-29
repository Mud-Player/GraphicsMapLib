#include "mappieitem.h"
#include "graphicsmap.h"

QSet<MapPieItem*> MapPieItem::m_items;

MapPieItem::MapPieItem() :
    m_coord({0, 0, 0}),
    m_radius(10e3),
    m_azimuth(0),
    m_span(0)
{
    // keep the outline width of 1-pixel when item scales
    auto pen = this->pen();
    pen.setColor(Qt::lightGray);
    pen.setWidth(1);
    pen.setCosmetic(true);
    this->setPen(pen);
    setAngle(60);
    //
    m_items.insert(this);
}

MapPieItem::~MapPieItem()
{
    m_items.remove(this);
}

void MapPieItem::setCoordinate(const QGeoCoordinate &coord)
{
    if(m_coord == coord)
        return;
    m_coord = coord;
    // such will change Rect
    updatePie();
}

void MapPieItem::setRadius(const qreal &meter)
{
    if(m_radius == meter)
        return;
    m_radius = meter;
    // such will change Rect
    updatePie();
}

void MapPieItem::setAzimuth(const qreal &degree)
{
    if(m_azimuth == m_azimuth)
        return;
    m_azimuth = degree;
    // aizmuth will not change Rect
    auto startAngle = m_azimuth - m_span / 2;
    this->setStartAngle(startAngle * 16);
    this->setSpanAngle(m_span * 16);
}

void MapPieItem::setAngle(const qreal &degree)
{
    if(m_span == degree)
        return;
    m_span = degree;
    // spanAngle will not change Rect
    auto startAngle = m_azimuth - m_span / 2;
    this->setStartAngle(startAngle * 16);
    this->setSpanAngle(m_span * 16);
}

void MapPieItem::updatePie()
{
    auto up = m_coord.atDistanceAndAzimuth(m_radius, 0);
    auto right = m_coord.atDistanceAndAzimuth(m_radius, 90);
    auto upPoint = GraphicsMap::toScene(up);
    auto rightPoint = GraphicsMap::toScene(right);
    auto centerPoint = GraphicsMap::toScene(m_coord);
    //
    QPointF topLeft(2*centerPoint.rx() - rightPoint.rx(), upPoint.ry());
    QPointF bottomRight(rightPoint.rx(), 2*centerPoint.ry() - upPoint.ry());
    QRectF rect(topLeft, bottomRight);
    this->setRect(rect);
}
