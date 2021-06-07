#include "interactivemap.h"
#include "mapellipseitem.h"
#include "mappolygonitem.h"
#include "maprouteitem.h"
#include "mapobjectitem.h"
#include "maprangeringitem.h"
#include "maptrailitem.h"
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
    return item;
}

void InteractiveMap::addMapEllipse(int id, MapEllipseItem *item)
{
    if(m_ellipseHash.contains(id))
        delete m_ellipseHash.take(id);
    m_ellipseHash.insert(id, item);
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
    return item;
}

void InteractiveMap::addMapObject(int id, MapObjectItem *item)
{
    if(m_objectHash.contains(id))
        delete m_objectHash.take(id);
    m_objectHash.insert(id, item);
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
    return item;
}

void InteractiveMap::addMapPolygon(int id, MapPolygonItem *item)
{
    if(m_polygonHash.contains(id))
        delete m_polygonHash.take(id);
    m_polygonHash.insert(id, item);
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
    return item;
}

void InteractiveMap::addMapRangeRing(int id, MapRangeRingItem *item)
{
    if(m_rangeRingHash.contains(id))
        delete m_rangeRingHash.take(id);
    m_rangeRingHash.insert(id, item);
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
    return item;
}

void InteractiveMap::addMapRoute(int id, MapRouteItem *item)
{
    if(m_routeHash.contains(id))
        delete m_routeHash.take(id);
    m_routeHash.insert(id, item);
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
    return item;
}

void InteractiveMap::addMapTrail(int id, MapTrailItem *item)
{
    if(m_trailHash.contains(id))
        delete m_trailHash.take(id);
    m_trailHash.insert(id, item);
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

void InteractiveMap::setOperator(InteractiveMapOperator *op)
{
    if(op == m_operator)
        return;
    // process end funtion with previos operator
    if(m_operator)
        m_operator->end();
    //
    op->setScene(this->scene());
    op->setMap(this);
    m_operator = op;
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
    m_object = nullptr;
    m_route = nullptr;
    m_finishRequested = false;
}

void MapObjectOperator::end()
{
    if(m_route)
        m_route->setEditable(false);
    m_object = nullptr;
    m_route = nullptr;
}

bool MapObjectOperator::keyPressEvent(QKeyEvent *event)
{
    if(!m_object)
        return false;
    if(event->key() == Qt::Key_Backspace) {
        m_route->remove(m_route->points().size()-1);
    }
    return false;
}

bool MapObjectOperator::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    // the last point have been craeted since previous mouse release event
    if(m_object) {
        m_finishRequested = true;
    }
    return false;
}

bool MapObjectOperator::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton) {
        m_finishRequested = true;
        return false;
    }
    m_pressPos = event->pos();
    return false;
}

bool MapObjectOperator::mouseReleaseEvent(QMouseEvent *event)
{
    // create end
    if(m_finishRequested) {
        m_finishRequested = false;
        if(m_route)
            m_route->setEditable(false);
        m_route = nullptr;
        m_object = nullptr;
        return false;
    }
    // do nothing
    if(m_pressPos != event->pos())
        return false;
    // create begin or append
    auto coord = m_map->toCoordinate(event->pos());
    if(!m_object) { // create object only
        m_object = new MapObjectItem;
        m_scene->addItem(m_object);
        m_object->setZValue(1);
        m_object->setCoordinate(coord);
        //
        emit created(m_object);
    }
    else { // we need to set route for object
        if(!m_route) { // create route
            m_route = new MapRouteItem;
            m_route->append({m_object->coordinate(), 0});
            m_scene->addItem(m_route);
            //
            emit created(m_route);
        }
        // append coordinate for route
        m_route->append({coord, 0});
    }
    return false;
}
