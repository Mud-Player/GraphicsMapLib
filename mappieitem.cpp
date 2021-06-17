#include "mappieitem.h"

MapPieItem::MapPieItem()
{

}

void MapPieItem::setCoordinate(const QGeoCoordinate &coord)
{
    if(m_coord == coord)
        return;
    m_coord = coord;
}

void MapPieItem::setDistance(const qreal &meter)
{
    if(m_radius == meter)
        return;
    m_radius = meter;
}

void MapPieItem::setRadius(const qreal &meter)
{
    if(m_distance == meter)
        return;
    m_distance = meter;
}
