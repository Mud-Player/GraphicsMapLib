#include "maprouteitem.h"
#include "graphicsmap.h"
#include "mapobjectitem.h"

QSet<MapRouteItem*> MapRouteItem::m_items;

MapRouteItem::MapRouteItem() :
    m_moveable(false),
    m_checkable(false),
    m_exclusive(true)
{
    //
    m_normalPen = this->pen();
    m_normalPen.setWidth(2);
    m_normalPen.setCosmetic(true);  // it will be always 2 pixmap whatever scale transform
    m_normalPen.setCapStyle(Qt::RoundCap);
    m_normalPen.setJoinStyle(Qt::RoundJoin);
    m_normalPen.setColor(QColor::fromRgb(241, 196, 15));
    m_moveablePen = m_normalPen;
    m_moveablePen.setColor(m_normalPen.color().lighter(170));
    this->setPen(m_normalPen);
    //
    m_items.insert(this);
}

MapRouteItem::~MapRouteItem()
{
    m_items.remove(this);
}

void MapRouteItem::setMoveable(bool movable)
{
    if(m_moveable == movable)
        return;
    m_moveable = movable;
    for(auto point : m_points) {
        point->setMovable(m_moveable);
        this->setPen(m_moveable ? m_moveablePen : m_normalPen);
    }
}

void MapRouteItem::setCheckable(bool checkable)
{
    // make sure call setChecked before operator=
    if(m_checkable == checkable)
        return;
    m_checkable = checkable;
}

void MapRouteItem::setChecked(int index)
{
    if(!m_checkable)
        return;
    if(m_exclusive) {
        for(auto point : m_points) {
            point->setChecked(false);
        }
    }
    m_points.at(index)->setChecked(true);
}

void MapRouteItem::setChecked(MapObjectItem *point)
{
    auto index = m_points.indexOf(point);
    setChecked(index);
}

void MapRouteItem::setExclusive(bool exclusive)
{
    if(m_exclusive == exclusive)
        return;
    for(auto point : m_points) {
        point->setChecked(false);
    }
}

MapObjectItem* MapRouteItem::append(MapObjectItem *point)
{
    bindPoint(point);
    m_points.append(point);
    // update point item pos
    //
    updatePolyline();
    //
    emit added(m_points.size()-1, point);
    return point;
}

MapObjectItem *MapRouteItem::append(const QGeoCoordinate &coord)
{
    auto point = new MapObjectItem(coord);
    append(point);
    return point;
}

MapObjectItem *MapRouteItem::insert(const int &index, MapObjectItem *point)
{
    bindPoint(point);
    m_points.insert(index, point);
    //
    updatePolyline();
    //
    emit added(index, point);
    return point;
}

MapObjectItem *MapRouteItem::insert(const int &index, const QGeoCoordinate &coord)
{
    auto point = new MapObjectItem(coord);
    insert(index, point);
    return point;
}

MapObjectItem *MapRouteItem::replace(const int &index, MapObjectItem *point)
{
    if(m_points.value(index) == point)
        return m_points.value(index);

    //
    delete m_points.value(index);
    bindPoint(point);
    m_points.replace(index, point);
    updatePolyline();
    //
    emit updated(index, point);
    return point;
}

MapObjectItem *MapRouteItem::replace(const int &index, const QGeoCoordinate &coord)
{
    auto point = new MapObjectItem(coord);
    replace(index, point);
    return point;
}

void MapRouteItem::remove(int index)
{
    if(index <0 || index >= m_points.size())
        return;

    delete m_points.takeAt(index);
    updatePolyline();
    //
    emit removed(index);
}

void MapRouteItem::remove(MapObjectItem *point)
{
    auto index = m_points.indexOf(point);
    remove(index);
}

const QVector<MapObjectItem*> &MapRouteItem::setPoints(const QVector<MapObjectItem *> &points)
{
    if(points == m_points)
        return m_points;

    // delete previous
    qDeleteAll(m_points);

    // make up the newly
    m_points = points;
    updatePolyline();
    //
    emit changed();
    return m_points;
}

const QVector<MapObjectItem *> &MapRouteItem::points() const
{
    return m_points;
}

QVector<MapObjectItem *> MapRouteItem::checked() const
{
    QVector<MapObjectItem*> checked;
    for(auto point : m_points) {
        if(point->isChecked())
            checked.append(point);
    }
    return checked;
}

QVector<int> MapRouteItem::checkedIndex() const
{
    QVector<int> checked;
    for(int i = 0; i < m_points.size(); ++i) {
        auto point = m_points.at(i);
        if(!point->isChecked())
            continue;
        checked.append(i);
    }
    return checked;
}

int MapRouteItem::indexOf(MapObjectItem *point)
{
    return m_points.indexOf(point);
}

const QSet<MapRouteItem *> &MapRouteItem::items()
{
    return m_items;
}

/// 更新QPainterPath的路径
/// 更新从beginIndex和endIndex之间多个航点的文字
void MapRouteItem::updatePolyline()
{
    // update path
    if(m_points.isEmpty()) {
        setPath(QPainterPath());
        return;
    }
    QPainterPath path(GraphicsMap::toScene(m_points.first()->coordinate()));
    for(int i = 1; i < m_points.size(); ++i) {
        path.lineTo(GraphicsMap::toScene(m_points.at(i)->coordinate()));
    }
    setPath(path);
}

void MapRouteItem::updatePointMoved()
{
    auto ctrlItem = dynamic_cast<MapObjectItem*>(sender());
    auto index = m_points.indexOf(ctrlItem);
    auto point = m_points.at(index);
    // update the polyline
    updatePolyline();
    emit updated(index, point);
}

void MapRouteItem::updatePointChecked()
{
    auto point = dynamic_cast<MapObjectItem*>(sender());
    setChecked(point);
}

void MapRouteItem::bindPoint(MapObjectItem *point)
{
    point->setParentItem(this);
    point->setMovable(m_moveable);
    point->setCheckable(m_checkable);
    point->setAllowMouseEvent(false);   // which case only press event will be received
    connect(point, &MapObjectItem::pressed, this, &MapRouteItem::updatePointChecked);
    connect(point, &MapObjectItem::coordinateChanged, this, &MapRouteItem::updatePointMoved);
}
