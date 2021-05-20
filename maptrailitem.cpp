#include "maptrailitem.h"
#include "graphicsmap.h"

MapTrailItem::MapTrailItem()
{
    auto pen = this->pen();
    pen.setWidth(2);
    pen.setCosmetic(true);  // it will be always 5 pixmap whatever scale transform
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setColor(Qt::yellow);
    this->setPen(pen);
}

void MapTrailItem::addCoordinate(const QGeoCoordinate &coord)
{
    auto path = this->path();
    if(!m_coord.isValid()) {
        auto point = GraphicsMap::fromCoordinate(coord);
        path.moveTo(point);
        m_coord = coord;
        setPath(path);
        return;
    }
    //
    if(m_coord.distanceTo(coord) < 50)
        return;
    //
    m_coord = coord;
    auto point = GraphicsMap::fromCoordinate(coord);
    path.lineTo(point);
    setPath(path);
}

void MapTrailItem::clear()
{
    setPath(QPainterPath());
    m_coord = QGeoCoordinate();
}
