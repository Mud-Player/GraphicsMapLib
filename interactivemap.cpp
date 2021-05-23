#include "interactivemap.h"
#include "mapellipseitem.h"
#include <QDebug>

InteractiveMap::InteractiveMap(QGraphicsScene *scene) : GraphicsMap(scene),
    m_operator(nullptr)
{

}

void InteractiveMap::setOperator(InteractiveMapOperator *op)
{
    op->setScene(this->scene());
    op->setMap(this);
    m_operator = op;
}

void InteractiveMap::wheelEvent(QWheelEvent *e)
{
    bool increase = e->angleDelta().y() > 0;
    if(increase)
        this->setZoomLevel(zoomLevel() + 0.2);
    else
        this->setZoomLevel(zoomLevel() - 0.2);
    e->accept();
}

void InteractiveMap::keyPressEvent(QKeyEvent *event)
{
    if(!m_operator || !m_operator->keyPressEvent(event))
        GraphicsMap::keyPressEvent(event);
}

void InteractiveMap::keyReleaseEvent(QKeyEvent *event)
{
    if(!m_operator || !m_operator->keyReleaseEvent(event))
        GraphicsMap::keyReleaseEvent(event);
}

void InteractiveMap::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!m_operator || !m_operator->mouseDoubleClickEvent(event))
        GraphicsMap::mouseDoubleClickEvent(event);
}

void InteractiveMap::mouseMoveEvent(QMouseEvent *event)
{
    // TODO: move event will be generated whether press event is triggerd or not
    if(!event->buttons() || !m_operator || !m_operator->mouseMoveEvent(event))
        GraphicsMap::mouseMoveEvent(event);
}

void InteractiveMap::mousePressEvent(QMouseEvent *event)
{
    if(!m_operator || !m_operator->mousePressEvent(event))
        GraphicsMap::mousePressEvent(event);
}

void InteractiveMap::mouseReleaseEvent(QMouseEvent *event)
{
    if(!m_operator || !m_operator->mouseReleaseEvent(event))
        GraphicsMap::mouseReleaseEvent(event);
}

MapEllipseOperator::MapEllipseOperator():
    m_ellipse(nullptr)
{
}

void MapEllipseOperator::ready()
{
    m_ellipse = nullptr;
}

void MapEllipseOperator::end()
{
    if(m_ellipse)
        m_ellipse->setEditable(false);
}

bool MapEllipseOperator::mousePressEvent(QMouseEvent *event)
{
    // Ignore the event whec click on the control point
    if(auto ctrlPoint = dynamic_cast<QGraphicsEllipseItem*>(m_map->itemAt(event->pos()))) {
        auto cast = dynamic_cast<MapEllipseItem*>(ctrlPoint->parentItem());
        if(MapEllipseItem::items().contains(cast)) {
            m_ignoreEvent = true;
            return false;
        }
    }

    // unset editable for previous created item
    if(m_ellipse)
        m_ellipse->setEditable(false);

    //
    m_ignoreEvent = false;
    m_ellipse = new MapEllipseItem;
    m_ellipse->setEditable(true);
    m_scene->addItem(m_ellipse);
    m_first  = m_map->toCoordinate(event->pos());
    m_ellipse->setRect(m_first, m_first);
    return true;
}

bool MapEllipseOperator::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_ignoreEvent)
        return false;
    auto second =  m_map->toCoordinate(event->pos());
    // Check that if the two point is too close, we should delete such an ellipse
    auto point0 = m_map->toPoint(m_first);
    auto point1 = m_map->toPoint(second);
    if((point0 - point1).manhattanLength() < 50) {
        delete m_ellipse;
        m_ellipse = nullptr;
        return true;
    }
    m_ellipse->setRect(m_first, second);
    return true;
}

bool MapEllipseOperator::mouseMoveEvent(QMouseEvent *event)
{
    if(m_ignoreEvent)
        return false;
    auto second =  m_map->toCoordinate(event->pos());
    m_ellipse->setRect(m_first, second);
    // Press Event didn't propagte to QGraphicsView ,
    // so we should to return false that helps up to zooming on cursor,
    // and map will not be moved by cursor move
    return false;
}
