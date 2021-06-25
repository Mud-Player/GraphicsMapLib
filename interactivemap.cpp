#include "interactivemap.h"
#include "mapellipseitem.h"
#include "mappolygonitem.h"
#include "maprouteitem.h"
#include "mapobjectitem.h"
#include "maprangeringitem.h"
#include "maptrailitem.h"
#include "maplineitem.h"
#include <QDebug>

InteractiveMap::InteractiveMap(QGraphicsScene *scene, QWidget *parent) : GraphicsMap(scene, parent),
    m_operator(nullptr),
    m_centerObj(nullptr),
    m_scaleable(true)
{

}

MapEllipseItem *InteractiveMap::addMapEllipse(int id)
{
    if(m_ellipseHash.contains(id))
        delete m_ellipseHash.take(id);
    auto item = new MapEllipseItem;
    m_ellipseHash.insert(id, item);
    scene()->addItem(item);
    return item;
}

void InteractiveMap::addMapEllipse(int id, MapEllipseItem *item)
{
    if(m_ellipseHash.contains(id))
        delete m_ellipseHash.take(id);
    m_ellipseHash.insert(id, item);
    if(item->scene() != scene())
        scene()->addItem(item);
}

bool InteractiveMap::removeMapEllipse(int id)
{
    auto item = m_ellipseHash.take(id);
    if(!item)
        return false;
    delete item;
    return true;
}

void InteractiveMap::clearMapEllipse()
{
    qDeleteAll(m_ellipseHash.values());
    m_ellipseHash.clear();
}

MapObjectItem *InteractiveMap::addMapObject(int id)
{
    if(m_objectHash.contains(id))
        delete m_objectHash.take(id);
    auto item = new MapObjectItem;
    m_objectHash.insert(id, item);
    scene()->addItem(item);
    return item;
}

void InteractiveMap::addMapObject(int id, MapObjectItem *item)
{
    if(m_objectHash.contains(id))
        delete m_objectHash.take(id);
    m_objectHash.insert(id, item);
    if(item->scene() != scene())
        scene()->addItem(item);
}

MapObjectItem *InteractiveMap::getMapObject(int id)
{
    return m_objectHash.value(id);
}

bool InteractiveMap::removeMapObject(int id)
{
    auto item = m_objectHash.take(id);
    if(!item)
        return false;
    delete item;
    return true;
}

void InteractiveMap::clearMapObject()
{
    qDeleteAll(m_objectHash.values());
    m_objectHash.clear();
}

MapPolygonItem *InteractiveMap::addMapPolygon(int id)
{
    if(m_polygonHash.contains(id))
        delete m_polygonHash.take(id);
    auto item = new MapPolygonItem;
    m_polygonHash.insert(id, item);
    scene()->addItem(item);
    return item;
}

void InteractiveMap::addMapPolygon(int id, MapPolygonItem *item)
{
    if(m_polygonHash.contains(id))
        delete m_polygonHash.take(id);
    m_polygonHash.insert(id, item);
    if(item->scene() != scene())
        scene()->addItem(item);
}

MapPolygonItem *InteractiveMap::getMapPolygon(int id)
{
    return m_polygonHash.value(id);
}

bool InteractiveMap::removeMapPolygon(int id)
{
    auto item = m_polygonHash.take(id);
    if(!item)
        return false;
    delete item;
    return true;
}

void InteractiveMap::clearMapPolygon()
{
    qDeleteAll(m_polygonHash.values());
    m_polygonHash.clear();
}

MapRangeRingItem *InteractiveMap::addMapRangeRing(int id)
{
    if(m_rangeRingHash.contains(id))
        delete m_rangeRingHash.take(id);
    auto item = new MapRangeRingItem;
    m_rangeRingHash.insert(id, item);
    scene()->addItem(item);
    return item;
}

void InteractiveMap::addMapRangeRing(int id, MapRangeRingItem *item)
{
    if(m_rangeRingHash.contains(id))
        delete m_rangeRingHash.take(id);
    m_rangeRingHash.insert(id, item);
    if(item->scene() != scene())
        scene()->addItem(item);
}

MapRangeRingItem *InteractiveMap::getMapRangeRing(int id)
{
    return m_rangeRingHash.value(id);
}

bool InteractiveMap::removeMapRangeRing(int id)
{
    auto item = m_rangeRingHash.take(id);
    if(!item)
        return false;
    delete item;
    return true;
}

void InteractiveMap::clearMapRangeRing()
{
    qDeleteAll(m_rangeRingHash.values());
    m_rangeRingHash.clear();
}

MapRouteItem *InteractiveMap::addMapRoute(int id)
{
    if(m_routeHash.contains(id))
        delete m_routeHash.take(id);
    auto item = new MapRouteItem;
    m_routeHash.insert(id, item);
    scene()->addItem(item);
    return item;
}

void InteractiveMap::addMapRoute(int id, MapRouteItem *item)
{
    if(m_routeHash.contains(id))
        delete m_routeHash.take(id);
    m_routeHash.insert(id, item);
    if(item->scene() != scene())
        scene()->addItem(item);
}

MapRouteItem *InteractiveMap::getMapRoute(int id)
{
    return m_routeHash.value(id);
}

bool InteractiveMap::removeMapRoute(int id)
{
    auto item = m_routeHash.take(id);
    if(!item)
        return false;
    delete item;
    return true;
}

void InteractiveMap::clearMapRoute()
{
    qDeleteAll(m_routeHash.values());
    m_routeHash.clear();
}

MapTrailItem *InteractiveMap::addMapTrail(int id)
{
    if(m_trailHash.contains(id))
        delete m_trailHash.take(id);
    auto item = new MapTrailItem;
    m_trailHash.insert(id, item);
    scene()->addItem(item);
    return item;
}

void InteractiveMap::addMapTrail(int id, MapTrailItem *item)
{
    if(m_trailHash.contains(id))
        delete m_trailHash.take(id);
    m_trailHash.insert(id, item);
    if(item->scene() != scene())
        scene()->addItem(item);
}

MapTrailItem *InteractiveMap::getMapTrail(int id)
{
    return m_trailHash.value(id);
}

bool InteractiveMap::removeMapTrail(int id)
{
    auto item = m_trailHash.take(id);
    if(!item)
        return false;
    delete item;
    return true;
}

void InteractiveMap::clearMapTrail()
{
    qDeleteAll(m_trailHash.values());
    m_trailHash.clear();
}

MapLineItem * InteractiveMap::addMapLine(int id)
{
	if (m_lineHash.contains(id))
		delete m_trailHash.take(id);
	auto item = new MapLineItem;
	m_lineHash.insert(id, item);
	scene()->addItem(item);
	return item;
}

void InteractiveMap::addMapLine(int id, MapLineItem * item)
{
	if (m_lineHash.contains(id))
		delete m_lineHash.take(id);
	m_lineHash.insert(id, item);
	scene()->addItem(item);
}

MapLineItem * InteractiveMap::getMapLine(int id)
{
	return m_lineHash.value(id);
}

bool InteractiveMap::removeLine(int id)
{
	auto item = m_lineHash.take(id);
	if (!item)
		return false;
	delete item;
	return true;
}

void InteractiveMap::clearMapLine()
{
	qDeleteAll(m_lineHash.values());
	m_lineHash.clear();
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

void InteractiveMap::clear()
{
    // IMPORT
    if(m_operator) m_operator->end();

    clearMapEllipse();
    clearMapObject();
    clearMapPolygon();
    clearMapRangeRing();
    clearMapRoute();
    clearMapTrail();

    // IMPORT
    if(m_operator) m_operator->ready();
}

void InteractiveMap::setCenter(const MapObjectItem *obj)
{
    if(m_centerObj)
        disconnect(obj, &MapObjectItem::coordinateChanged, this, &GraphicsMap::centerOn);
    // only case that we center on object at first that we should to save drag mode and anchor mode
    else {
        m_dragMode = this->dragMode();
        m_anchor = this->transformationAnchor();
    }
    //
    m_centerObj = obj;
    if(m_centerObj) {
        // We should not to drag map when object is always center on map
        connect(obj, &MapObjectItem::coordinateChanged, this, &GraphicsMap::centerOn);
        this->setDragMode(QGraphicsView::NoDrag);
        this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    }
    // restore previous drag mode if centerOn is unset
    else {
        this->setDragMode(m_dragMode);
        this->setTransformationAnchor(m_anchor);
    }
}

void InteractiveMap::setScaleable(bool on)
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
