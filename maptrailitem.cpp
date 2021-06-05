﻿#include "maptrailitem.h"
#include "graphicsmap.h"
#include "mapobjectitem.h"

MapTrailItem::MapTrailItem() :
    m_attachObj(nullptr)
{
    auto pen = this->pen();
    pen.setWidth(2);
    pen.setCosmetic(true);  // it will be always 2 pixmap whatever scale transform
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setColor(Qt::yellow);
    this->setPen(pen);
}

void MapTrailItem::addCoordinate(const QGeoCoordinate &coord)
{
    auto path = this->path();
    if(!m_coord.isValid()) {
        auto point = GraphicsMap::toScene(coord);
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
    auto point = GraphicsMap::toScene(coord);
    path.lineTo(point);
    setPath(path);
}

void MapTrailItem::clear()
{
    setPath(QPainterPath());
    m_coord = QGeoCoordinate();
}

void MapTrailItem::attach(MapObjectItem *obj)
{
    if(m_attachObj)
        disconnect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapTrailItem::addCoordinate);
    //
    clear();
    m_attachObj = obj;
    if(m_attachObj)
        connect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapTrailItem::addCoordinate);
}

void MapTrailItem::detach()
{
    if(m_attachObj)
        disconnect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapTrailItem::addCoordinate);
    m_attachObj = nullptr;
    clear();
}
