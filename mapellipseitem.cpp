#include "mapellipseitem.h"
#include "graphicsmap.h"
#include <QPen>

MapEllipseItem::MapEllipseItem():
    m_center(0,0),
    m_size(1e3, 1e3)
{
    // keep the outline width of 1-pixel when item scales
    auto pen = this->pen();
    pen.setWidth(0);
    setPen(pen);
}

void MapEllipseItem::setEditable(const bool &editable)
{
    if(m_editable == editable)
        return;

    m_editable = editable;
    updateEditable();
}

void MapEllipseItem::setCenter(const QGeoCoordinate &center)
{
    if(center == m_center)
        return;
    m_center = center;
    updateEllipse();
}

void MapEllipseItem::setSize(const QSizeF &size)
{
    if(m_size == size)
        return;
    m_size = size;
    updateEllipse();
}

void MapEllipseItem::setRect(const QGeoCoordinate &first, const QGeoCoordinate &second)
{
    // compute the left right top bottom, which help up to compute the center and the width&height
    double left, right, top, bottom;
    if(first.longitude() <= second.longitude()) {
        left = first.longitude();
        right = second.longitude();
    } else {
        left = second.longitude();
        right = first.longitude();
    }
    if(first.latitude() <= second.latitude()) {
        bottom = first.latitude();
        top = second.latitude();
    } else {
        bottom = second.latitude();
        top = first.longitude();
    }
    // ignore setter requeset if shape and position has no differece
    QGeoCoordinate center((left+right)/2, (top+right)/2);
    auto width = QGeoCoordinate(center.latitude(), left).distanceTo(QGeoCoordinate(center.latitude(), right));
    auto height = QGeoCoordinate(bottom, center.longitude()).distanceTo(QGeoCoordinate(top, center.longitude()));
    QSizeF size(width, height);
    if(center == m_center && size == m_size)
        return;

    // rebuild ellipse shape
    m_center = center;
    m_size = size;
    updateEllipse();
}

void MapEllipseItem::updateEllipse()
{
    auto leftCoord = m_center.atDistanceAndAzimuth(m_size.width()/2, -90);
    auto topCoord = m_center.atDistanceAndAzimuth(m_size.height()/2, 0);
    auto center = MudMap::fromCoordinate(m_center);
    auto left = MudMap::fromCoordinate(leftCoord);
    auto top = MudMap::fromCoordinate(topCoord);
    auto halfWidth = left.x() - center.x();
    auto halfHeight = top.y() - center.y();
    QGraphicsEllipseItem::setRect(-halfWidth, -halfHeight, 2 * halfWidth, 2*halfHeight);
    setPos(center);
}

void MapEllipseItem::updateEditable()
{

}
