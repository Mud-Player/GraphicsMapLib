#include "interactivemap.h"
#include "mapellipseitem.h"
#include "mappolygonitem.h"
#include "maprouteitem.h"
#include "mapobjectitem.h"
#include "maprangeringitem.h"
#include "maptrailitem.h"
#include "maplineitem.h"
#include <QDebug>

InteractiveMap::InteractiveMap(QWidget *parent) : GraphicsMap(parent),
    m_operator(nullptr),
    m_centerObj(nullptr),
    m_scaleable(true)
{

}

void InteractiveMap::setOperator(InteractiveMapOperator *op)
{
    if(op == m_operator)
        return;
    // process end funtion with previos operator
    if(m_operator)
        m_operator->end();

    m_operator = op;
    if(!m_operator)
        return;
    //
    m_operator->setScene(this->scene());
    m_operator->setMap(this);
    // process ready funtion with newlly operator
    op->ready();
}

void InteractiveMap::setCenter(const MapObjectItem *obj)
{
    if(m_centerObj)
        disconnect(obj, &MapObjectItem::coordinateChanged, this, qOverload<const QGeoCoordinate&>(&GraphicsMap::centerOn));
    // only case that we center on object at first that we should to save drag mode and anchor mode
    else {
        m_dragMode = this->dragMode();
        m_anchor = this->transformationAnchor();
    }
    //
    m_centerObj = obj;
    if(m_centerObj) {
        // We should not to drag map when object is always center on map
        connect(obj, &MapObjectItem::coordinateChanged, this, qOverload<const QGeoCoordinate&>(&GraphicsMap::centerOn));
        this->setDragMode(QGraphicsView::NoDrag);
        this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
        centerOn(obj->coordinate());
    }
    // restore previous drag mode if centerOn is unset
    else {
        this->setDragMode(m_dragMode);
        this->setTransformationAnchor(m_anchor);
    }
}

void InteractiveMap::setZoomable(bool on)
{
    m_scaleable = on;
}

void InteractiveMap::wheelEvent(QWheelEvent *e)
{
    if(!m_scaleable) {
        e->accept();
        return;
    }
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
    viewport()->setCursor(Qt::ArrowCursor);
}

void InteractiveMap::mouseReleaseEvent(QMouseEvent *event)
{
    if(!m_operator || !m_operator->mouseReleaseEvent(event))
        GraphicsMap::mouseReleaseEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}
