#include "maprouteitem.h"
#include "graphicsmap.h"

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

MapRoutePoint* MapRouteItem::append(const MapRouteItem::Point &point)
{
    auto pointItem = createPoint();
    m_routePoints.append(point);
    m_ctrlItems.append(pointItem);
    updateRoute();
    //
    emit added(m_routePoints.size()-1, point);
    return pointItem;
}

MapRoutePoint *MapRouteItem::insert(const int &index, const MapRouteItem::Point &point)
{
    auto pointItem = createPoint();
    m_routePoints.insert(index, point);
    m_ctrlItems.insert(index, pointItem);
    updateRoute();
    //
    emit added(index, point);
    return pointItem;
}

MapRoutePoint *MapRouteItem::replace(const int &index, const MapRouteItem::Point &point)
{
    if(m_routePoints.value(index) == point)
        return m_ctrlItems.value(index);

    delete m_ctrlItems.value(index);
    //
    auto pointItem = createPoint();
    m_routePoints.replace(index, point);
    m_ctrlItems.replace(index, pointItem);
    updateRoute();
    //
    emit updated(index, point);
    return pointItem;
}

void MapRouteItem::remove(const int &index)
{
    if(index >= m_routePoints.size()-1)
        return;

    auto coord = m_routePoints.takeAt(index);
    delete m_ctrlItems.takeAt(index);
    updateRoute();
    //
    emit removed(index, coord);
}

const QVector<MapRoutePoint*> &MapRouteItem::setPoints(const QVector<MapRouteItem::Point> &points)
{
    if(points == m_routePoints)
        return m_ctrlItems;

    // delete previous
    for(const auto &item : m_ctrlItems) {
        delete item;
    }
    m_ctrlItems.clear();

    // make up the newly
    m_routePoints = points;
    for(int i = 0; i < points.size(); ++i) {
       m_ctrlItems.append(createPoint());
    }
    updateRoute();
    //
    emit changed();
    return m_ctrlItems;
}

const QVector<MapRoutePoint *> &MapRouteItem::points() const
{
    return m_ctrlItems;
}

/// 更新QPainterPath的路径
/// 更新多个航点的位置与文本
void MapRouteItem::updateRoute()
{
    // update path


}

MapRoutePoint *MapRouteItem::createPoint()
{
    auto pointItem = new MapRoutePoint;
    pointItem->setParentItem(this);
    pointItem->installSceneEventFilter(this);
    return pointItem;
}

MapRoutePoint::MapRoutePoint()
{
    this->setRect(-5, -5, 10, 10);
}

void MapRoutePoint::setText(const QString &text)
{
    m_text.setText(text);
}

void MapRoutePoint::setAnimation(bool on)
{

}
