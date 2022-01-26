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
    m_centerObj(nullptr),
    m_scaleable(true)
{

}

void InteractiveMap::pushOperator(InteractiveMapOperator *op)
{
    if(!op)
        return;
    // process end funtion with previos operator
    if(!m_operators.isEmpty()) {
        auto pre =  m_operators.top();
        pre->end();
        pre->m_ignoreKeyEventLoop = false;
        pre->m_ignoreMouseEventLoop = false;
    }

    m_operators.push(op);
    op->pushState();
    //
    op->setScene(this->scene());
    op->setMap(this);
    if(op->isTransient())
        connect(op, &InteractiveMapOperator::completed, this, &InteractiveMap::popOperator);

    // process ready funtion with newlly operator
    op->ready();
}

void InteractiveMap::popOperator()
{
    if(m_operators.isEmpty())
        return;
    // unset current
    auto op = m_operators.pop();
    op->popState();
    if(op->isTransient())
        disconnect(op, &InteractiveMapOperator::completed, this, &InteractiveMap::popOperator);
    op->end();
    op->m_ignoreKeyEventLoop = false;
    op->m_ignoreMouseEventLoop = false;

    // set new current
    // todo: ignore event for newOp at first
    if(!m_operators.isEmpty()) {
        auto newOp = m_operators.top();
        newOp->ready();
        newOp->m_ignoreKeyEventLoop = true;
        newOp->m_ignoreMouseEventLoop = true;
    }
}

void InteractiveMap::clearOperator()
{
    while (!m_operators.isEmpty()) {
        popOperator();
    }
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
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    if(!op || op->isIgnoreKeyEventLoop() || !op->keyPressEvent(event))
        GraphicsMap::keyPressEvent(event);
}

void InteractiveMap::keyReleaseEvent(QKeyEvent *event)
{
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    if(!op || op->isIgnoreKeyEventLoop() || !op->keyReleaseEvent(event))
        GraphicsMap::keyReleaseEvent(event);
    if(op)
        op->m_ignoreKeyEventLoop = false;
}

void InteractiveMap::mouseDoubleClickEvent(QMouseEvent *event)
{
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    if(!op || op->isIgnoreMouseEventLoop()  || !op->mouseDoubleClickEvent(event))
        GraphicsMap::mouseDoubleClickEvent(event);
}

void InteractiveMap::mouseMoveEvent(QMouseEvent *event)
{
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    // TODO: move event will be generated whether press event is triggerd or not
    if(!event->buttons()  || !op || op->isIgnoreMouseEventLoop() || !op->mouseMoveEvent(event))
        GraphicsMap::mouseMoveEvent(event);
}

void InteractiveMap::mousePressEvent(QMouseEvent *event)
{
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    if(!op || op->isIgnoreMouseEventLoop() || !op->mousePressEvent(event))
        GraphicsMap::mousePressEvent(event);

    viewport()->setCursor(Qt::ArrowCursor);
}

void InteractiveMap::mouseReleaseEvent(QMouseEvent *event)
{
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    if(!op || op->isIgnoreMouseEventLoop() || !op->mouseReleaseEvent(event))
        GraphicsMap::mouseReleaseEvent(event);
    if(op)
        op->m_ignoreMouseEventLoop = false;

    viewport()->setCursor(Qt::ArrowCursor);
}
