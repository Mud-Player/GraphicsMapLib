#include "mappolygonitem.h"
#include "graphicsmap.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>

QSet<MapPolygonItem*> MapPolygonItem::m_items;

MapPolygonItem::MapPolygonItem() :
    m_editable(false),
    m_sceneAdded(false)
{
    // keep the outline width of 1-pixel when item scales
    auto pen = this->pen();
    pen.setWidth(1);
    pen.setCosmetic(true);
    this->setPen(pen);
    //
    m_items.insert(this);
    updateEditable();
}

MapPolygonItem::~MapPolygonItem()
{
    m_items.remove(this);
    qDeleteAll(m_ctrlPoints);
    m_ctrlPoints.clear();
}

void MapPolygonItem::setEditable(bool editable)
{
    if(m_editable == editable)
        return;

    m_editable = editable;
    emit editableChanged(editable);
    updateEditable();
}

bool MapPolygonItem::isEditable() const
{
    return m_editable;
}

void MapPolygonItem::toggleEditable()
{
    setEditable(!m_editable);
}

void MapPolygonItem::append(const QGeoCoordinate &coord)
{
    m_coords.append(coord);
    // Adding scene point
    auto point = GraphicsMap::toScene(coord);
    m_points.append(point);
    updatePolygon();
    //
    emit added(m_points.size()-1, coord);
}

void MapPolygonItem::replace(const int &index, const QGeoCoordinate &coord)
{
    if(m_coords.at(index) == coord)
        return;
    m_coords.replace(index, coord);
    // Modify scene point
    auto point = GraphicsMap::toScene(coord);
    m_points.replace(index, point);
    updatePolygon();
    //
    emit updated(index, coord);
}

void MapPolygonItem::remove(int index)
{
    if(index < 0 || index >= m_coords.size())
        return;
    auto coord = m_coords.at(index);
    m_coords.removeAt(index);
    m_points.removeAt(index);
    //
    updatePolygon();
    //
    emit removed(index, coord);
}

void MapPolygonItem::removeEnd()
{
    remove(m_coords.size() - 1);
}

void MapPolygonItem::setPoints(const QVector<QGeoCoordinate> &coords)
{
    if(m_coords == coords)
        return;

    // Change previous coords and points
    m_coords = coords;
    m_points.clear();
    for(auto &coord : coords) {
        auto point = GraphicsMap::toScene(coord);
        m_points.append(point);
    }
    updatePolygon();
    //
    emit changed();
}

const QVector<QGeoCoordinate> &MapPolygonItem::points() const
{
    return m_coords;
}

int MapPolygonItem::count()
{
    return m_coords.size();
}

const QGeoCoordinate &MapPolygonItem::at(int i) const
{
    return m_coords.at(i);
}

const QSet<MapPolygonItem *> &MapPolygonItem::items()
{
    return m_items;
}

/// the function will take advantage of those Ctrl-Points's Position property,
/// also, the function will determine the outlook of those Ctrl-Points
bool MapPolygonItem::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if(!m_editable)
        return false;
    auto ctrlPoint = dynamic_cast<QGraphicsEllipseItem*>(watched);
    switch (event->type()) {
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
    {
        // get the index which respond to polygon edge point
        int index = m_ctrlPoints.indexOf(ctrlPoint);
        // actully, the finally event which release event will adjust the correct position
        m_points.replace(index, watched->pos());
        this->setPolygon(m_points);
        // it's required to updadte the scene transform result
        auto scenePos = watched->pos();
        auto coord = GraphicsMap::toCoordinate(scenePos);
        m_coords.replace(index, coord);
        //
        emit updated(index, coord);
        break;
    }
    case QEvent::GraphicsSceneHoverEnter:
    {
        ctrlPoint->setBrush(Qt::white);
        ctrlPoint->setScale(1.2);
        break;
    }
    case QEvent::GraphicsSceneHoverLeave:
    {
        ctrlPoint->setBrush(Qt::lightGray);
        ctrlPoint->setScale(1);
        break;
    }
    default:
        break;
    }

    return false;
}

QVariant MapPolygonItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
   if(change != ItemSceneHasChanged)
       return QGraphicsPolygonItem::itemChange(change, value);

   m_sceneAdded = true;
   for(auto ctrlPoint : qAsConst(m_ctrlPoints)) {
       // control point's move event help us to move polygon point
       ctrlPoint->installSceneEventFilter(this);
   }
   return QGraphicsPolygonItem::itemChange(change, value);
}

void MapPolygonItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPolygonItem::mouseDoubleClickEvent(event);
    emit doubleClicked();
}

void MapPolygonItem::updatePolygon()
{
    // Reset polygon data to QGraphicsPolygonItem
    this->setPolygon(m_points);

    // Create new control point or Remove unused control point
    if(m_ctrlPoints.size() < m_points.size()) { // Create
        for(int i = m_ctrlPoints.size(); i < m_points.size(); ++i) {
            auto ctrlPoint = new QGraphicsEllipseItem;
            ctrlPoint->setParentItem(this);
            //
            ctrlPoint->setAcceptHoverEvents(true);
            ctrlPoint->setPen(QPen(Qt::gray));
            ctrlPoint->setBrush(Qt::lightGray);
            ctrlPoint->setCursor(Qt::DragMoveCursor);
            ctrlPoint->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            ctrlPoint->setFlag(QGraphicsItem::ItemIsMovable);
            ctrlPoint->setRect(-4, -4, 8, 8);

            // control point's move event help us to move polygon point
            if(m_sceneAdded)
                ctrlPoint->installSceneEventFilter(this);

            //
            m_ctrlPoints.append(ctrlPoint);
        }
    }
    else if(m_points.size() < m_ctrlPoints.size()) { // Create
        for(int i = m_points.size(); i < m_ctrlPoints.size(); ++i) {
            delete m_ctrlPoints.takeLast();
        }
    }

    // Move Control Point to GeoCoordinate's postion
    for(int i = 0; i < m_points.size(); ++i) {
        m_ctrlPoints.at(i)->setPos(m_points.at(i));
    }
    //
    updateEditable();
}

void MapPolygonItem::updateEditable()
{
    auto pen = this->pen();
    pen.setColor(m_editable ? Qt::white : Qt::lightGray);
    this->setPen(pen);

    for(auto &ctrlPoint : m_ctrlPoints) {
        ctrlPoint->setVisible(m_editable);
    }
}
