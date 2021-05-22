#include "mapellipseoperator.h"
#include "mapellipseitem.h"
#include <QDebug>

MapEllipseOperator::MapEllipseOperator()
{

}

bool MapEllipseOperator::mousePressEvent(QMouseEvent *event)
{
    m_ellipse = new MapEllipseItem;
    m_scene->addItem(m_ellipse);
    m_first  = m_map->toCoordinate(event->pos());
    m_ellipse->setRect(m_first, m_first);
    return true;
}

bool MapEllipseOperator::mouseReleaseEvent(QMouseEvent *event)
{
    auto second =  m_map->toCoordinate(event->pos());
    m_ellipse->setRect(m_first, second);
    return true;
}

