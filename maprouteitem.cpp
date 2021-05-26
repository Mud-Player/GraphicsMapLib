#include "maprouteitem.h"
#include "graphicsmap.h"

bool MapRouteItem::Point::operator==(const MapRouteItem::Point &rhs)
{
    return (this->coord == rhs.coord && this->speed == rhs.speed);
}

MapRouteItem::MapRouteItem()
{
    qRegisterMetaType<MapRouteItem::Point>("MapRouteItem::Point");
    //
    auto pen = this->pen();
    pen.setWidth(2);
    pen.setCosmetic(true);  // it will be always 2 pixmap whatever scale transform
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setColor(Qt::yellow);
    this->setPen(pen);
}

void MapRouteItem::append(const MapRouteItem::Point &point)
{
    m_points.append(point);
    updateRoute();
    //
    emit added(m_points.size()-1, point);
}

void MapRouteItem::insert(const int &index, const MapRouteItem::Point &point)
{
    m_points.insert(index, point);
    updateRoute();
    //
    emit added(index, point);
}

void MapRouteItem::replace(const int &index, const MapRouteItem::Point &point)
{
    if(m_points.value(index) == point)
        return;
    m_points.replace(index, point);
    updateRoute();
    //
    emit updated(index, point);
}

void MapRouteItem::remove(const int &index)
{
    auto coord = m_points.takeAt(index);
    updateRoute();
    //
    emit removed(index, coord);
}

void MapRouteItem::setPoints(const QVector<MapRouteItem::Point> &points)
{
    if(points == m_points)
        return;
    m_points = points;
    updateRoute();
    //
    emit changed();
}
