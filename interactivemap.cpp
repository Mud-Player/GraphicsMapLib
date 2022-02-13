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

bool InteractiveMap::pushOperator(MapOperator *op)
{
    if(!op)
        return false;
    else if(m_operators.contains(op))
        return false;
    // process end funtion with previos operator
    if(!m_operators.isEmpty()) {
        auto pre =  m_operators.top();
        if(pre->mode() == MapOperator::EditOnly)
            popOperator();
        else
            pre->end();
    }

    m_operators.push(op);
    //
    op->setScene(this->scene());
    op->setMap(this);
    if(op->mode() == MapOperator::EditOnly)
        connect(op, &MapOperator::completed, this, &InteractiveMap::popOperator, Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
    connect(op, &MapOperator::modeChanged, this, &InteractiveMap::onOperatorModeChanged, Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
    // process ready funtion with newlly operator
    op->ready();
    return true;
}

bool InteractiveMap::popOperator()
{
    if(m_operators.isEmpty())
        return false;
    // unset current
    auto op = m_operators.pop();
    disconnect(op, &MapOperator::completed, this, &InteractiveMap::popOperator);
    op->end();

    // set new current
    // todo: ignore event for newOp at first
    if(!m_operators.isEmpty()) {
        auto newOp = m_operators.top();
        newOp->ready();
    }
    return true;
}

MapOperator *InteractiveMap::topOperator() const
{
    if(m_operators.isEmpty())
        return nullptr;
    return m_operators.top();
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
        connect(obj, &MapObjectItem::coordinateChanged, this, qOverload<const QGeoCoordinate&>(&GraphicsMap::centerOn), Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
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
    if(!op || !op->handleKeyPressEvent(event))
        GraphicsMap::keyPressEvent(event);
}

void InteractiveMap::keyReleaseEvent(QKeyEvent *event)
{
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    if(!op || !op->handleKeyReleaseEvent(event))
        GraphicsMap::keyReleaseEvent(event);
}

void InteractiveMap::mouseMoveEvent(QMouseEvent *event)
{
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    // TODO: move event will be generated whether press event is triggerd or not
    if( !op || !op->handleMouseMoveEvent(event))
        GraphicsMap::mouseMoveEvent(event);
}

void InteractiveMap::mousePressEvent(QMouseEvent *event)
{
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    if(!op || !op->handleMousePressEvent(event))
        GraphicsMap::mousePressEvent(event);

    viewport()->setCursor(Qt::ArrowCursor);
}

void InteractiveMap::mouseReleaseEvent(QMouseEvent *event)
{
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    if(!op || !op->handleMouseReleaseEvent(event))
        GraphicsMap::mouseReleaseEvent(event);

    viewport()->setCursor(Qt::ArrowCursor);
}

void InteractiveMap::mouseDoubleClickEvent(QMouseEvent *event)
{
    auto op = m_operators.isEmpty() ? nullptr : m_operators.top();
    if(!op || !op->handleMouseDoubleClickEvent(event))
        GraphicsMap::mouseDoubleClickEvent(event);
}


void InteractiveMap::onOperatorModeChanged()
{
    auto op = dynamic_cast<MapOperator*>(sender());
    if(op->mode() == MapOperator::EditOnly) {
        connect(op, &MapOperator::completed, this, &InteractiveMap::popOperator, Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
    }
    else {
        disconnect(op, &MapOperator::completed, this, &InteractiveMap::popOperator);
    }
}

MapOperator::MapOperator(QObject *parent) : QObject(parent)
{

}

void MapOperator::setMode(OperatorMode mode)
{
    if(mode == m_mode)
        return;
    m_mode = mode;
    emit modeChanged(mode);
}

void MapOperator::ready()
{

}

void MapOperator::end()
{
    // waitting press event to active @enable
    m_keyEventEnable = false;
    m_mouseEventEnable = false;
}

bool MapOperator::handleKeyPressEvent(QKeyEvent *event)
{
    m_keyEventEnable = true;
    return keyPressEvent(event);
}

bool MapOperator::handleKeyReleaseEvent(QKeyEvent *event)
{
    if(m_skipOnceKeyEvent) {
        m_skipOnceKeyEvent = false;
        return false;
    }
    else if(m_keyEventEnable)
        return keyReleaseEvent(event);
    return false;
}

bool MapOperator::handleMousePressEvent(QMouseEvent *event)
{
    // press event means a new event round
    m_mouseEventEnable = true;
    m_mouseMoveEventEnable = true;
    return mousePressEvent(event);
}

bool MapOperator::handleMouseMoveEvent(QMouseEvent *event)
{
    // we should propagate event when we in case of mouseTracking
    if(m_mouseMoveTrackingEnable) {
       return mouseMoveEvent(event);
    }
    else if(m_mouseEventEnable && m_mouseMoveEventEnable && !m_skipOnceMouseEvent)
       return mouseMoveEvent(event);

    return false;
}

bool MapOperator::handleMouseReleaseEvent(QMouseEvent *event)
{
    m_mouseMoveEventEnable = false;
    if(m_skipOnceMouseEvent) {
        m_skipOnceMouseEvent = false;
        return false;
    }
    else if(m_mouseEventEnable) {
        return mouseReleaseEvent(event);
    }
    return false;
}

bool MapOperator::handleMouseDoubleClickEvent(QMouseEvent *event)
{
    m_mouseMoveEventEnable = true;
    if(m_skipOnceMouseEvent) {
        return false;
    }
    else if(m_mouseEventEnable)
        return mouseDoubleClickEvent(event);
    return false;
}
