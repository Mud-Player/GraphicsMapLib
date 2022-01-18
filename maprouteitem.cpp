#include "maprouteitem.h"
#include "graphicsmap.h"
#include "mapobjectitem.h"

QSet<MapRouteItem*> MapRouteItem::m_items;

MapRouteItem::MapRouteItem() :
    m_editable(false),
    m_checkable(false),
    m_checkedIndex(-1)
{
    //
    m_normalPen = this->pen();
    m_normalPen.setWidth(2);
    m_normalPen.setCosmetic(true);  // it will be always 2 pixmap whatever scale transform
    m_normalPen.setCapStyle(Qt::RoundCap);
    m_normalPen.setJoinStyle(Qt::RoundJoin);
    m_normalPen.setColor(QColor::fromRgb(241, 196, 15));
    m_editablePen = m_normalPen;
    m_editablePen.setColor(m_normalPen.color().lighter(170));
    this->setPen(m_normalPen);
    //
    m_items.insert(this);
}

MapRouteItem::~MapRouteItem()
{
    m_items.remove(this);
}

void MapRouteItem::setEditable(const bool &editable)
{
    if(m_editable == editable)
        return;
    m_editable = editable;

    this->setPen(m_editable ? m_editablePen : m_normalPen);
    for(auto &ctrlPoint : m_points) {
        ctrlPoint->setMovable(editable);
    }
}

void MapRouteItem::setCheckable(const bool &checkable)
{
    // make sure call setChecked before operator=
    if(!checkable)
        setChecked(-1);
    m_checkable = checkable;
}

void MapRouteItem::setChecked(int index)
{
    if(m_checkedIndex == index || !m_checkable)
        return;
    auto prePoint = m_points.value(m_checkedIndex);
    if(prePoint)
        prePoint->setChecked(false);
    auto curPoint = m_points.value(index);
    if(m_checkable && curPoint)
        curPoint->setChecked(true);
    m_checkedIndex = index;
}

int MapRouteItem::checked() const
{
    return m_checkedIndex;
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

void MapRouteItem::remove(const int &index)
{
    if(index <0 || index >= m_points.size())
        return;

    delete m_points.takeAt(index);
    updatePolyline();
    //
    emit removed(index);
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

void MapRouteItem::updateByPointMove()
{
    auto ctrlItem = dynamic_cast<MapObjectItem*>(sender());
    auto index = m_points.indexOf(ctrlItem);
    auto point = m_points.at(index);
    // update the polyline
    updatePolyline();
    emit updated(index, point);
}

void MapRouteItem::checkByPointPress()
{
    auto ctrlItem = dynamic_cast<MapObjectItem*>(sender());
    auto index = m_points.indexOf(ctrlItem);
    setChecked(index);
}

void MapRouteItem::bindPoint(MapObjectItem *point)
{
    point->setParentItem(this);
    point->setMovable(m_editable);
    point->setCheckable(true);
    point->setClickable(true);
    connect(point, &MapObjectItem::pressed, this, &MapRouteItem::checkByPointPress);
    connect(point, &MapObjectItem::coordinateChanged, this, &MapRouteItem::updateByPointMove);
}
