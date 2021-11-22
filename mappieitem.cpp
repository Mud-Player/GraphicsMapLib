#include "mappieitem.h"
#include "graphicsmap.h"
#include "mapobjectitem.h"

QSet<MapPieItem*> MapPieItem::m_items;
QSet<MapTriTrapItem*> MapTriTrapItem::m_items;

MapPieItem::MapPieItem() :
    m_coord({0, 0, 0}),
    m_radius(10e3),
    m_azimuth(0),
    m_span(0)
{
    // keep the outline width of 1-pixel when item scales
    auto pen = this->pen();
    pen.setColor(Qt::lightGray);
    pen.setWidth(1);
    pen.setCosmetic(true);
    this->setPen(pen);
    setAngle(60);
    //
    m_items.insert(this);
}

MapPieItem::~MapPieItem()
{
    m_items.remove(this);
}

void MapPieItem::setCoordinate(const QGeoCoordinate &coord)
{
    if(m_coord == coord)
        return;
    m_coord = coord;
    // such will change Rect
    updatePie();
}

void MapPieItem::setRadius(const qreal &meter)
{
    if(m_radius == meter)
        return;
    m_radius = meter;
    // such will change Rect
    updatePie();
}

void MapPieItem::setAzimuth(const qreal &degree)
{
    if(m_azimuth == degree)
        return;
    m_azimuth = degree;
    // aizmuth will not change Rect
    auto startAngle = m_azimuth - m_span / 2;
    this->setStartAngle(startAngle * 16);
    this->setSpanAngle(m_span * 16);
}

void MapPieItem::setAngle(const qreal &degree)
{
    if(m_span == degree)
        return;
    m_span = degree;
    // spanAngle will not change Rect
    auto startAngle = m_azimuth - m_span / 2;
    this->setStartAngle(startAngle * 16);
    this->setSpanAngle(m_span * 16);
}

void MapPieItem::updatePie()
{
    auto up = m_coord.atDistanceAndAzimuth(m_radius, 0);
    auto right = m_coord.atDistanceAndAzimuth(m_radius, 90);
    auto upPoint = GraphicsMap::toScene(up);
    auto rightPoint = GraphicsMap::toScene(right);
    auto centerPoint = GraphicsMap::toScene(m_coord);
    //
    QPointF topLeft(2*centerPoint.rx() - rightPoint.rx(), upPoint.ry());
    QPointF bottomRight(rightPoint.rx(), 2*centerPoint.ry() - upPoint.ry());
    QRectF rect(topLeft, bottomRight);
    this->setRect(rect);
}

MapTriTrapItem::MapTriTrapItem():
    m_coord({0, 0, 0}),
    m_near(10e3),
    m_far(20e3),
    m_azimuth(0),
    m_span(0),
    m_attachObj(nullptr),
    m_attachAzimuth(0)
{
    // keep the outline width of 1-pixel when item scales
    auto pen = this->pen();
    pen.setColor(Qt::NoPen);
    pen.setWidth(1);
    pen.setCosmetic(true);
    this->setPen(pen);
    //
    m_triangle.setParentItem(this);
    m_trapezoid.setParentItem(this);
    pen.setColor(Qt::lightGray);
    pen.setStyle(Qt::DotLine);
    m_triangle.setPen(pen);
    pen.setColor(Qt::red);
    pen.setStyle(Qt::SolidLine);
    m_trapezoid.setPen(pen);
    setAngle(60);
    //
    m_items.insert(this);
}

MapTriTrapItem::~MapTriTrapItem()
{
    m_items.remove(this);
}

void MapTriTrapItem::setCoordinate(const QGeoCoordinate &coord)
{
    if(m_coord == coord)
        return;
    m_coord = coord;
    // such will change Rect
    updateTrapezoid();
}

void MapTriTrapItem::setNear(const qreal &meter)
{
    if(m_near == meter)
        return;
    m_near = meter;
    //
    updateTrapezoid();
}

void MapTriTrapItem::setAzimuth(const qreal &degree)
{
    if(m_azimuth == degree)
        return;
    m_azimuth = degree;
    //
    updateTrapezoid();
}

void MapTriTrapItem::setAngle(const qreal &degree)
{
    if(m_span == degree)
        return;
    m_span = degree;
    //
    updateTrapezoid();
}

QGraphicsPolygonItem *MapTriTrapItem::getTriangle()
{
    return &m_triangle;
}

QGraphicsPolygonItem *MapTriTrapItem::getTrapezoid()
{
    return &m_trapezoid;
}

void MapTriTrapItem::attach(MapObjectItem *obj)
{
    if(m_attachObj) {
        disconnect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapTriTrapItem::setCoordinate);
        disconnect(m_attachObj, &MapObjectItem::rotationChanged, this, &MapTriTrapItem::on_attachRotationChanged);
    }
    if(obj) {
        m_attachObj = obj;
        connect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapTriTrapItem::setCoordinate);
        connect(m_attachObj, &MapObjectItem::rotationChanged, this, &MapTriTrapItem::on_attachRotationChanged);
        setCoordinate(m_attachObj->coordinate());
        on_attachRotationChanged(m_attachObj->rotation());
    }
}

void MapTriTrapItem::detach()
{
    if(m_attachObj) {
        disconnect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapTriTrapItem::setCoordinate);
        disconnect(m_attachObj, &MapObjectItem::rotationChanged, this, &MapTriTrapItem::on_attachRotationChanged);
        m_attachAzimuth = 0;
    }
    m_attachObj = nullptr;
}

void MapTriTrapItem::updateTrapezoid()
{
    auto span_2 = m_span / 2;
    auto beginAz = m_azimuth + m_attachAzimuth - span_2;
    auto endAz = m_azimuth + m_attachAzimuth + span_2;
    auto coord1 = m_coord.atDistanceAndAzimuth(m_near, beginAz);
    auto coord2 = m_coord.atDistanceAndAzimuth(m_near, endAz);
    auto coord3 = m_coord.atDistanceAndAzimuth(m_far, endAz);
    auto coord4 = m_coord.atDistanceAndAzimuth(m_far, beginAz);
    auto point0 = GraphicsMap::toScene(m_coord);
    auto point1 = GraphicsMap::toScene(coord1);
    auto point2 = GraphicsMap::toScene(coord2);
    auto point3 = GraphicsMap::toScene(coord3);
    auto point4 = GraphicsMap::toScene(coord4);
    {   // Self
        QPolygonF polygon;
        polygon.append(point0);
        polygon.append(point3);
        polygon.append(point4);
        this->setPolygon(polygon);
    }
    {   // Triangle
        QPolygonF polygon;
        polygon.append(point0);
        polygon.append(point1);
        polygon.append(point2);
        m_triangle.setPolygon(polygon);
    }
    {   // Trapozoid
        QPolygonF polygon;
        polygon.append(point1);
        polygon.append(point2);
        polygon.append(point3);
        polygon.append(point4);
        m_trapezoid.setPolygon(polygon);
    }
}

void MapTriTrapItem::on_attachRotationChanged(const qreal &degree)
{
    if(m_attachAzimuth == degree)
        return;
    m_attachAzimuth = degree;
    updateTrapezoid();
}
