#include "mappolygonitem.h"
#include "graphicsmap.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>

/*!
 * \details 只要顶点有修改，就会重新构建多边形与控制点，是否有好的优化方案?
 * 以后实际使用过程中，看一下对性能影响大不大
 */
MapPolygonItem::MapPolygonItem() :
    m_editable(true)
{
}

void MapPolygonItem::setEditable(const bool &editable)
{
    if(m_editable == editable)
        return;

    m_editable = editable;
    updateEditable();
}

void MapPolygonItem::addCoordinate(const QGeoCoordinate &coord)
{
    m_coords.append(coord);
    // Adding scene point
    auto point = GraphicsMap::fromCoordinate(coord);
    m_points.append(point);
    updatePolygon();
    //
    emit added(m_points.size()-1, coord);
}

void MapPolygonItem::replaceCoordinate(const int &index, const QGeoCoordinate &coord)
{
    if(m_coords.at(index) == coord)
        return;
    m_coords.replace(index, coord);
    // Modify scene point
    auto point = GraphicsMap::fromCoordinate(coord);
    m_points.replace(index, point);
    updatePolygon();
    //
    emit updated(index, coord);
}

void MapPolygonItem::removeCoordinate(const int &index)
{
    auto coord = m_coords.at(index);
    m_coords.removeAt(index);
    m_points.removeAt(index);
    //
    updatePolygon();
    //
    emit removed(index, coord);
}

void MapPolygonItem::setCoordinates(const QVector<QGeoCoordinate> &coords)
{
    if(m_coords == coords)
        return;

    // Change previous coords and points
    m_coords = coords;
    m_points.clear();
    for(auto &coord : coords) {
        auto point = GraphicsMap::fromCoordinate(coord);
        m_points.append(point);
    }
    updatePolygon();
    //
    emit changed();
}

const QVector<QGeoCoordinate> &MapPolygonItem::coordinates() const
{
    return m_coords;
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
        m_coords.replace(index, GraphicsMap::toCoordinate(scenePos));
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
            ctrlPoint->setCursor(Qt::DragMoveCursor);
            ctrlPoint->setRect(-4, -4, 8, 8);
            // control point's move event help us to remove polygon
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
    pen.setWidth(0);
    pen.setColor(m_editable ? Qt::white : Qt::lightGray);
    this->setPen(pen);

    for(auto &ctrlPoint : m_ctrlPoints) {
        ctrlPoint->setVisible(m_editable);
    }
}
