#include "maprouteitem.h"
#include "graphicsmap.h"

QSet<MapRouteItem*> MapRouteItem::m_items;

MapRouteItem::MapRouteItem() :
    m_editable(true),
    m_sceneAdded(false),
    m_autoNumber(true)
{
    qRegisterMetaType<MapRouteItem::Point>("MapRouteItem::Point");
    //
    auto pen = this->pen();
    pen.setWidth(2);
    pen.setCosmetic(true);  // it will be always 2 pixmap whatever scale transform
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setColor(Qt::white);
    this->setPen(pen);
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

    auto pen = this->pen();
    pen.setColor(m_editable ? Qt::white : Qt::lightGray);
    this->setPen(pen);
    for(auto &ctrlPoint : m_ctrlItems) {
        ctrlPoint->setFlag(QGraphicsItem::ItemIsMovable, editable);
        ctrlPoint->setCursor(editable ? Qt::DragMoveCursor : Qt::ArrowCursor);
    }
}

void MapRouteItem::setAutoNumber(bool on)
{
    m_autoNumber = on;
}

MapRoutePoint* MapRouteItem::append(const MapRouteItem::Point &point)
{
    auto ctrlPoint = createPoint();
    m_routePoints.append(point);
    m_ctrlItems.append(ctrlPoint);
    // update point item pos
    ctrlPoint->setPos(GraphicsMap::toScene(point.coord));
    //
    updatePolylineAndText(m_routePoints.size()-1, m_routePoints.size()-1);
    //
    emit added(m_routePoints.size()-1, point);
    return ctrlPoint;
}

MapRoutePoint *MapRouteItem::insert(const int &index, const MapRouteItem::Point &point)
{
    auto ctrlPoint = createPoint();
    m_routePoints.insert(index, point);
    m_ctrlItems.insert(index, ctrlPoint);
    // update point item pos
    ctrlPoint->setPos(GraphicsMap::toScene(point.coord));
    //
    updatePolylineAndText(index, m_routePoints.size()-1);
    //
    emit added(index, point);
    return ctrlPoint;
}

MapRoutePoint *MapRouteItem::replace(const int &index, const MapRouteItem::Point &point)
{
    if(m_routePoints.value(index) == point)
        return m_ctrlItems.value(index);

    //
    m_routePoints.replace(index, point);
    // update point item pos
    auto ctrlPoint = m_ctrlItems.value(index);;
    ctrlPoint->setPos(GraphicsMap::toScene(point.coord));
    //
    updatePolylineAndText(index, index);
    //
    emit updated(index, point);
    return ctrlPoint;
}

void MapRouteItem::remove(const int &index)
{
    if(index >= m_routePoints.size()-1)
        return;

    auto coord = m_routePoints.takeAt(index);
    delete m_ctrlItems.takeAt(index);
    updatePolylineAndText(index, m_routePoints.size()-1);
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
        auto ctrlPoint = createPoint();
        m_ctrlItems.append(ctrlPoint);
        ctrlPoint->setPos(GraphicsMap::toScene(points.at(i).coord));
    }
    //
    updatePolylineAndText(0, m_routePoints.size()-1);
    //
    emit changed();
    return m_ctrlItems;
}

const QVector<MapRoutePoint *> &MapRouteItem::points() const
{
    return m_ctrlItems;
}

bool MapRouteItem::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if(!m_editable)
        return false;
    auto ctrlPoint = dynamic_cast<MapRoutePoint*>(watched);
    switch (event->type()) {
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
    {
        // get the index which respond to polygon edge point
        int index = m_ctrlItems.indexOf(ctrlPoint);
        // update the point data
        auto point = m_routePoints.at(index);
        point.coord = GraphicsMap::toCoordinate(ctrlPoint->pos());
        m_routePoints.replace(index, point);
        // update the polyline
        updatePolylineAndText(-1, -1);  // negative value means than only polyline will be updated
        //
        emit updated(index, point);
        break;
    }
    case QEvent::GraphicsSceneHoverEnter:
    {
        ctrlPoint->setBrush(Qt::white);
        break;
    }
    case QEvent::GraphicsSceneHoverLeave:
    {
        ctrlPoint->setBrush(Qt::lightGray);
        break;
    }
    default:
        break;
    }
    return false;
}

QVariant MapRouteItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
   if(change != ItemSceneHasChanged)
       return QGraphicsPathItem::itemChange(change, value);

   m_sceneAdded = true;
   for(auto ctrlPoint : m_ctrlItems) {
       // control point's move event help us to move polygon point
       ctrlPoint->installSceneEventFilter(this);
   }
   return QGraphicsPathItem::itemChange(change, value);
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

MapRoutePoint *MapRouteItem::createPoint()
{
    auto ctrlPoint = new MapRoutePoint;
    ctrlPoint->setParentItem(this);
    ctrlPoint->setAcceptHoverEvents(true);
    ctrlPoint->setFlag(QGraphicsItem::ItemIsMovable, m_editable);
    if(m_sceneAdded)
        ctrlPoint->installSceneEventFilter(this);
    return ctrlPoint;
}

MapRoutePoint::MapRoutePoint()
{
    this->setRect(-10, -10, 20, 20);
    this->setPen(QPen(Qt::darkGray));
    this->setBrush(Qt::lightGray);
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    //
    auto font = m_text.font();
    font.setFamily("Microsoft YaHei");
    m_text.setFont(font);
    m_text.setBrush(Qt::black);
    m_text.setParentItem(this);
}

void MapRoutePoint::setText(const QString &text)
{
    m_text.setText(text);
    m_text.setPos(-m_text.boundingRect().center());
}

void MapRoutePoint::setAnimation(bool on)
{
    Q_UNUSED(on)
}
