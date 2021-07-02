#include "maprouteitem.h"
#include "graphicsmap.h"
#include "mapobjectitem.h"

QSet<MapRouteItem*> MapRouteItem::m_items;

MapRouteItem::MapRouteItem() :
    m_editable(false),
    m_checkable(false),
    m_autoNumber(true),
    m_checkedIndex(-1)
{
    qRegisterMetaType<MapRouteItem::Point>("MapRouteItem::Point");
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
    for(auto &ctrlPoint : m_ctrlItems) {
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
    auto prePoint = m_ctrlItems.value(m_checkedIndex);
    if(prePoint)
        prePoint->setChecked(false);
    auto curPoint = m_ctrlItems.value(index);
    if(m_checkable && curPoint)
        curPoint->setChecked(true);
    m_checkedIndex = index;
}

int MapRouteItem::checked() const
{
    return m_checkedIndex;
}

void MapRouteItem::setAutoNumber(bool on)
{
    m_autoNumber = on;
}

MapObjectItem* MapRouteItem::append(const MapRouteItem::Point &point)
{
    auto ctrlPoint = createPoint(point.coord);
    m_routePoints.append(point);
    m_ctrlItems.append(ctrlPoint);
    // update point item pos
    //
    updatePolylineAndText(m_routePoints.size()-1, m_routePoints.size()-1);
    //
    emit added(m_routePoints.size()-1, point);
    return ctrlPoint;
}

MapObjectItem *MapRouteItem::insert(const int &index, const MapRouteItem::Point &point)
{
    auto ctrlPoint = createPoint(point.coord);
    m_routePoints.insert(index, point);
    m_ctrlItems.insert(index, ctrlPoint);
    //
    updatePolylineAndText(index, m_routePoints.size()-1);
    //
    emit added(index, point);
    return ctrlPoint;
}

MapObjectItem *MapRouteItem::replace(const int &index, const MapRouteItem::Point &point)
{
    if(m_routePoints.value(index) == point)
        return m_ctrlItems.value(index);

    //
    m_routePoints.replace(index, point);
    // update point item pos
    auto ctrlPoint = m_ctrlItems.value(index);;
    ctrlPoint->setCoordinate(point.coord);
    //
    updatePolylineAndText(index, index);
    //
    emit updated(index, point);
    return ctrlPoint;
}

void MapRouteItem::remove(const int &index)
{
    if(index <0 || index >= m_routePoints.size())
        return;

    auto coord = m_routePoints.takeAt(index);
    delete m_ctrlItems.takeAt(index);
    updatePolylineAndText(index, m_routePoints.size()-1);
    //
    emit removed(index, coord);
}

const QVector<MapObjectItem*> &MapRouteItem::setPoints(const QVector<MapRouteItem::Point> &points)
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
        auto ctrlPoint = createPoint(points.at(i).coord);
        m_ctrlItems.append(ctrlPoint);
    }
    //
    updatePolylineAndText(0, m_routePoints.size()-1);
    //
    emit changed();
    return m_ctrlItems;
}

const QVector<MapRouteItem::Point> &MapRouteItem::points() const
{
    return m_routePoints;
}

const QSet<MapRouteItem *> &MapRouteItem::items()
{
    return m_items;
}

/// 更新QPainterPath的路径
/// 更新从beginIndex和endIndex之间多个航点的文字
void MapRouteItem::updatePolylineAndText(int beginIndex, int endIndex)
{
    // update path
    if(m_routePoints.isEmpty()) {
        setPath(QPainterPath());
        return;
    }
    QPainterPath path(GraphicsMap::toScene(m_routePoints.first().coord));
    for(int i = 1; i < m_routePoints.size(); ++i) {
        path.lineTo(GraphicsMap::toScene(m_routePoints.at(i).coord));
    }
    setPath(path);

    // update point item text if coulud
    if(!m_autoNumber)
        return;
    for(int i = qMax(0, beginIndex); i < m_ctrlItems.size() && i <= endIndex; ++i) {
        m_ctrlItems.at(i)->setText(QString::number(i));
    }
}

void MapRouteItem::updateByPointMove()
{
    auto ctrlItem = dynamic_cast<MapObjectItem*>(sender());
    auto index = m_ctrlItems.indexOf(ctrlItem);
    auto point = m_routePoints.at(index);
    auto alt = point.coord.altitude();
    point.coord = GraphicsMap::toCoordinate(ctrlItem->pos());
    point.coord.setAltitude(alt);
    m_routePoints.replace(index, point);
    // update the polyline
    updatePolylineAndText(-1, -1);  // negative value means than only polyline will be updated
    emit updated(index, point);
}

void MapRouteItem::checkByPointPress()
{
    auto ctrlItem = dynamic_cast<MapObjectItem*>(sender());
    auto index = m_ctrlItems.indexOf(ctrlItem);
    setChecked(index);
}

MapObjectItem *MapRouteItem::createPoint(const QGeoCoordinate &coord)
{
    auto ctrlPoint = new MapObjectItem;
    ctrlPoint->setParentItem(this);
    ctrlPoint->setIcon({":/Resources/dot.png"});
    ctrlPoint->setCoordinate(coord);
    ctrlPoint->setMovable(m_editable);
    ctrlPoint->setCheckable(true);
    ctrlPoint->setClickable(true);
    connect(ctrlPoint, &MapObjectItem::pressed, this, &MapRouteItem::checkByPointPress);
    connect(ctrlPoint, &MapObjectItem::coordinateChanged, this, &MapRouteItem::updateByPointMove);
    return ctrlPoint;
}
