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
    updatePolygon();
}

void MapPolygonItem::replaceCoordinate(const int &index, const QGeoCoordinate &coord)
{
    if(m_coords.at(index) == coord)
        return;
    m_coords.replace(index, coord);
    updatePolygon();
}

void MapPolygonItem::removeCoordinate(const int &index)
{
    m_coords.removeAt(index);
    updatePolygon();
}

void MapPolygonItem::setCoordinates(const QList<QGeoCoordinate> &coords)
{
    if(m_coords == coords)
        return;

    m_coords = coords;
    updatePolygon();
}

const QList<QGeoCoordinate> &MapPolygonItem::coordinates() const
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
        auto polygon = this->polygon();
        // actully, the finally event which release event will adjust the correct position
        polygon.replace(index, watched->pos());
        setPolygon(polygon);
        // it's required to updadte the local result
        auto scenePos = watched->pos() + m_center;
        m_coords.replace(index, MudMap::toCoordinate(scenePos));
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
    QPolygonF posPath;
    for(auto &coord : m_coords) {
        posPath.append(MudMap::fromCoordinate(coord));
    }
    // We have to move all points to around center
    // Which help us to implement the fix-scale function
    auto center = posPath.boundingRect().center();
    for(auto &pos : posPath) {
        pos -= center;
    }
    QGraphicsPolygonItem::setPolygon(posPath);
    setPos(center);

    // Clear control points, and set control points
    for(auto &ctrlPoint : m_ctrlPoints) {
        delete ctrlPoint;
    }
    m_ctrlPoints.clear();

    for(auto &pos : posPath) {
        auto ctrlPoint = new QGraphicsEllipseItem;
        ctrlPoint->setParentItem(this);
        // We need some hover effect
        ctrlPoint->setAcceptHoverEvents(true);
        ctrlPoint->setPen(QPen(Qt::gray));
        ctrlPoint->setBrush(Qt::lightGray);
        ctrlPoint->setCursor(Qt::DragMoveCursor);
        ctrlPoint->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        ctrlPoint->setFlag(QGraphicsItem::ItemIsMovable);
        ctrlPoint->setCursor(Qt::DragMoveCursor);
        // TODO: EllipseItem should work with a coordinate in parent coordinate system
        ctrlPoint->setRect(-4, -4, 8, 8);
        ctrlPoint->setPos(pos);
        // install event helper
        ctrlPoint->installSceneEventFilter(this);
        m_ctrlPoints.append(ctrlPoint);
    }
    updateEditable();
}

void MapPolygonItem::updateEditable()
{
    auto pen = this->pen();
    pen.setWidth(0);
    pen.setColor(m_editable ? Qt::white : Qt::lightGray);
    setPen(pen);

    for(auto &ctrlPoint : m_ctrlPoints) {
        ctrlPoint->setVisible(m_editable);
    }
}
