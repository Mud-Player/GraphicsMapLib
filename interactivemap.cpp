#include "interactivemap.h"
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
    if(!m_operator || !m_operator->mouseMoveEvent(event))
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
