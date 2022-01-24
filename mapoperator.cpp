#include "mapoperator.h"
#include "mapellipseitem.h"
#include "mappolygonitem.h"
#include "maprouteitem.h"
#include "mapobjectitem.h"
#include "maprangeringitem.h"
#include "maptrailitem.h"
#include "maplineitem.h"
#include "maprectitem.h"
#include <QDebug>

InteractiveMapOperator::InteractiveMapOperator(QObject *parent) : QObject(parent)
{

}

MapEllipseOperator::MapEllipseOperator(QObject *parent) : InteractiveMapOperator(parent)
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
    m_scene->addItem(m_ellipse);
    m_first  = m_map->toCoordinate(event->pos());
    m_ellipse->setRect(m_first, m_first);
    m_ellipse->setEditable(true);
    //
    emit created(m_ellipse);
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

MapPolygonOperator::MapPolygonOperator(QObject *parent) : InteractiveMapOperator(parent)
{

}

void MapPolygonOperator::ready()
{
    m_polygon = nullptr;
    m_finishRequested = false;
}

void MapPolygonOperator::end()
{
    if(m_polygon)
        m_polygon->setEditable(false);
}

bool MapPolygonOperator::keyPressEvent(QKeyEvent *event)
{
    if(!m_polygon)
        return false;
    if(event->key() == Qt::Key_Backspace) {
        m_polygon->remove(m_polygon->points().size()-1);
    }
    return false;
}

bool MapPolygonOperator::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    // the last point have been craeted since previous mouse release event
    if(m_polygon) {
        m_finishRequested = true;
    }
    return false;
}

bool MapPolygonOperator::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton) {
        m_finishRequested = true;
        return false;
    }
    m_pressPos = event->pos();
    return false;
}

bool MapPolygonOperator::mouseReleaseEvent(QMouseEvent *event)
{
    // create end
    if(m_finishRequested) {
        m_finishRequested = false;
        if(m_polygon)
            m_polygon->setEditable(false);
        m_polygon = nullptr;
        return false;
    }
    // do nothing
    if(m_pressPos != event->pos())
        return false;
    // create begin or append
    if(!m_polygon) {
        m_polygon = new MapPolygonItem;
        m_polygon->setEditable(true);
        m_scene->addItem(m_polygon);
        //
        emit created(m_polygon);
    }
    m_polygon->append(m_map->toCoordinate(event->pos()));
    return false;
}

MapObjectOperator::MapObjectOperator(QObject *parent) : InteractiveMapOperator(parent)
{
}

void MapObjectOperator::ready()
{
}

void MapObjectOperator::end()
{
}

bool MapObjectOperator::mousePressEvent(QMouseEvent *event)
{
    if(!(event->buttons() & Qt::LeftButton)) {
        return false;
    }
    m_pressPos = event->pos();
    return false;
}

bool MapObjectOperator::mouseReleaseEvent(QMouseEvent *event)
{
    // do nothing
    if(m_pressPos != event->pos())
        return false;

    // create
    auto coord = m_map->toCoordinate(event->pos());
    auto object = new MapObjectItem;
    m_scene->addItem(object);
    object->setZValue(1);
    object->setCoordinate(coord);
    //
    emit created(object);
    emit completed();
    return false;
}

MapRouteOperator::MapRouteOperator(QObject *parent) : InteractiveMapOperator(parent)
{
}

void MapRouteOperator::edit(MapRouteItem *item)
{
    if(m_route) {
        m_route->setMoveable(false);
        m_route->setCheckable(false);
    }
    m_route = item;
    m_route->setMoveable(true);
    m_route->setCheckable(true);
}

void MapRouteOperator::setWaypointIcon(const QPixmap &pixmap)
{
    m_waypointIcon = pixmap;
}

void MapRouteOperator::ready()
{
    m_route = nullptr;
}

void MapRouteOperator::end()
{
    if(m_route) {
        m_route->setMoveable(false);
        m_route->setCheckable(false);
    }
    m_route = nullptr;
}

bool MapRouteOperator::keyPressEvent(QKeyEvent *event)
{
    if(!m_route)
        return false;
    if(event->key() == Qt::Key_Backspace) {
        auto points = m_route->checked();
        for(auto point : points) {
            m_route->remove(point);
        }
    }
    return false;
}

bool MapRouteOperator::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    // the last point have been craeted since previous mouse release event
    // complete
    if(m_route) {
        m_route->setMoveable(false);
        m_route->setCheckable(false);
    }
    m_route = nullptr;
    emit completed();
    return false;
}

bool MapRouteOperator::mousePressEvent(QMouseEvent *event)
{
    // complete
    if(event->buttons() & Qt::RightButton) {
        if(m_route) {
            m_route->setMoveable(false);
            m_route->setCheckable(false);
        }
        m_route = nullptr;
        ignoreMouseEventLoop();
        emit completed();
        return false;
    }
    m_pressPos = event->pos();
    if(!m_route)
        return false;

    // check that if we clicked waypoint
    auto mouseItems = m_map->items(event->pos());
    auto routeItems = m_route->childItems();
    for(auto item : mouseItems) {
        // find MapObjectItem child
        if(m_route->childItems().contains(item)) {
            ignoreMouseEventLoop();
            return false;   // the waypoint itself will process setChecked
        }
    }
    return false;
}

bool MapRouteOperator::mouseReleaseEvent(QMouseEvent *event)
{
    // do nothing
    if((m_pressPos-event->pos()).manhattanLength() > 3)
        return false;

    // create begin or append
    auto coord = m_map->toCoordinate(event->pos());
    coord.setAltitude(0);
    if(!m_route) { // create route
        m_route = new MapRouteItem;
        m_scene->addItem(m_route);
        m_route->setMoveable(true);
        m_route->setCheckable(true);
        //
        emit created(m_route);
    }
    auto checked = m_route->checkedIndex();
    auto index = checked.isEmpty() ? -1 : checked.last();
    // append coordinate for route
    m_route->insert(index + 1, coord)->setIcon(m_waypointIcon);
    m_route->setChecked(index + 1);
    return false;
}
MapRangeLineOperator::MapRangeLineOperator(QObject *parent) : InteractiveMapOperator(parent)
{
}

void MapRangeLineOperator::ready()
{
	m_line = nullptr;
	m_ignoreEvent = false;
}

void MapRangeLineOperator::end()
{
	m_line = nullptr;
}

bool MapRangeLineOperator::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
		m_pressFirstPos = event->pos();

		if (!m_line) { // create route
			m_line = new MapLineItem;
			m_scene->addItem(m_line);
			m_line->setStartPoint(m_map->toCoordinate(event->pos()));
            m_line->setStartIcon(QPixmap(":/Resources/location.png"), Qt::AlignHCenter | Qt::AlignTop);
            m_line->setEndIcon(QPixmap(":/Resources/location.png"), Qt::AlignHCenter | Qt::AlignTop);
			//
			emit created(m_line);
			m_ignoreEvent = false;
		}
		return true;
	}
	return false;
}

bool MapRangeLineOperator::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
	if (m_line) { // delete line
		m_ignoreEvent = true;
		m_line = nullptr;
		return true;
	}
	return false;
}

bool MapRangeLineOperator::mouseMoveEvent(QMouseEvent *event)
{
	if (m_ignoreEvent)
		return false;
	auto second = m_map->toCoordinate(event->pos());
	m_line->setEndPoint(second);
    double dis = m_line->endings().first.distanceTo(m_line->endings().second);
    if (dis > 1000.0) {
		dis = dis * 1E-3;
		QString str = QString::number(dis, 'f', 2) + QString("km");
		m_line->setText(str);
	}
    else if (dis < 1000.0) {
		QString str = QString::number(dis,'f',2) + QString("m");
		m_line->setText(str);
	}
	// Press Event didn't propagte to QGraphicsView ,
	// so we should to return false that helps up to zooming on cursor,
	// and map will not be moved by cursor move
	return false;
}

MapRectOperator::MapRectOperator(QObject *parent)
{

}

void MapRectOperator::ready()
{
    m_rect = nullptr;
}

void MapRectOperator::end()
{
    if(m_rect)
        m_rect->setEditable(false);
}

bool MapRectOperator::mousePressEvent(QMouseEvent *event)
{
    // Ignore the event whec click on the control point
    if(auto ctrlPoint = dynamic_cast<QGraphicsRectItem*>(m_map->itemAt(event->pos()))) {
        auto cast = dynamic_cast<MapRectItem*>(ctrlPoint->parentItem());
        if(MapRectItem::items().contains(cast)) {
            m_ignoreEvent = true;
            return false;
        }
    }

    // unset editable for previous created item
    if(m_rect)
        m_rect->setEditable(false);

    //
    m_ignoreEvent = false;
    m_rect = new MapRectItem;
    m_scene->addItem(m_rect);
    m_first  = m_map->toCoordinate(event->pos());
    m_rect->setRect(m_first, m_first);
    m_rect->setEditable(true);
    //
    emit created(m_rect);
    return true;
}

bool MapRectOperator::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_ignoreEvent)
        return false;
    auto second =  m_map->toCoordinate(event->pos());
    // Check that if the two point is too close, we should delete such an rect
    auto point0 = m_map->toPoint(m_first);
    auto point1 = m_map->toPoint(second);
    if((point0 - point1).manhattanLength() < 50) {
        delete m_rect;
        m_rect = nullptr;
        return true;
    }
    m_rect->setRect(m_first, second);
    emit completed();

    return true;
}

bool MapRectOperator::mouseMoveEvent(QMouseEvent *event)
{
    if(m_ignoreEvent)
        return false;
    auto second =  m_map->toCoordinate(event->pos());
    m_rect->setRect(m_first, second);
    // Press Event didn't propagte to QGraphicsView ,
    // so we should to return false that helps up to zooming on cursor,
    // and map will not be moved by cursor move
    return false;
}
