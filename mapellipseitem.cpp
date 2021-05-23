﻿#include "mapellipseitem.h"
#include "graphicsmap.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QPen>

QSet<MapEllipseItem*> MapEllipseItem::m_items;

MapEllipseItem::MapEllipseItem():
    m_center(0,0),
    m_size(1e3, 1e3)
{
    // keep the outline width of 1-pixel when item scales
    auto pen = this->pen();
    pen.setWidth(1);
    pen.setCosmetic(true);
    this->setPen(pen);
    //
    m_firstCtrl.setRect(-4, -4, 8, 8);
    m_secondCtrl.setRect(-4, -4, 8, 8);
    m_rectCtrl.setParentItem(this);
    m_firstCtrl.setParentItem(this);
    m_secondCtrl.setParentItem(this);
    //
    pen.setColor(Qt::lightGray);
    pen.setStyle(Qt::DashLine);
    m_rectCtrl.setPen(pen);
    m_firstCtrl.setAcceptHoverEvents(true);
    m_firstCtrl.setPen(QPen(Qt::gray));
    m_firstCtrl.setBrush(Qt::lightGray);
    m_firstCtrl.setCursor(Qt::DragMoveCursor);
    m_firstCtrl.setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_firstCtrl.setFlag(QGraphicsItem::ItemIsMovable);
    m_firstCtrl.setCursor(Qt::DragMoveCursor);
    m_secondCtrl.setAcceptHoverEvents(true);
    m_secondCtrl.setPen(QPen(Qt::gray));
    m_secondCtrl.setBrush(Qt::lightGray);
    m_secondCtrl.setCursor(Qt::DragMoveCursor);
    m_secondCtrl.setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_secondCtrl.setFlag(QGraphicsItem::ItemIsMovable);
    m_secondCtrl.setCursor(Qt::DragMoveCursor);
    //
    m_items.insert(this);
}

MapEllipseItem::~MapEllipseItem()
{
    m_items.remove(this);
}

void MapEllipseItem::setEditable(const bool &editable)
{
    if(m_editable == editable)
        return;

    m_editable = editable;
    updateEditable();
}

void MapEllipseItem::setCenter(const QGeoCoordinate &center)
{
    if(center == m_center)
        return;
    m_center = center;
    // We should to compute topleft and botoom right coordinate to keep previous size unchanged
    auto leftCoord = m_center.atDistanceAndAzimuth(m_size.width()/2, -90);
    auto rightCoord = m_center.atDistanceAndAzimuth(m_size.height()/2, 90);
    auto topCoord = m_center.atDistanceAndAzimuth(m_size.height()/2, 0);
    auto bottomCoord = m_center.atDistanceAndAzimuth(m_size.height()/2, 180);
    m_topLeftCoord = {topCoord.latitude(), leftCoord.longitude()};
    m_bottomRightCoord = {bottomCoord.latitude(), rightCoord.longitude()};
    updateEllipse();
    //
    emit centerChanged(center);
}

void MapEllipseItem::setSize(const QSizeF &size)
{
    if(m_size == size)
        return;
    m_size = size;
    // We should to compute topleft and botoom right coordinate from new size
    auto leftCoord = m_center.atDistanceAndAzimuth(m_size.width()/2, -90);
    auto rightCoord = m_center.atDistanceAndAzimuth(m_size.height()/2, 90);
    auto topCoord = m_center.atDistanceAndAzimuth(m_size.height()/2, 0);
    auto bottomCoord = m_center.atDistanceAndAzimuth(m_size.height()/2, 180);
    m_topLeftCoord = {topCoord.latitude(), leftCoord.longitude()};
    m_bottomRightCoord = {bottomCoord.latitude(), rightCoord.longitude()};
    updateEllipse();
    //
    emit sizeChanged(size);
}

void MapEllipseItem::setRect(const QGeoCoordinate &first, const QGeoCoordinate &second)
{
    // compute the left right top bottom, which help up to compute the center and the width&height
    double left, right, top, bottom;
    if(first.longitude() <= second.longitude()) {
        left = first.longitude();
        right = second.longitude();
    } else {
        left = second.longitude();
        right = first.longitude();
    }
    if(first.latitude() <= second.latitude()) {
        bottom = first.latitude();
        top = second.latitude();
    } else {
        bottom = second.latitude();
        top = first.latitude();
    }
    // ignore setter requeset if shape and position has no differece
    const QGeoCoordinate tlCoord(top, left);
    const QGeoCoordinate brCoord(bottom, right);
    if(tlCoord == m_topLeftCoord && brCoord == m_bottomRightCoord)
        return;
    m_topLeftCoord = tlCoord;
    m_bottomRightCoord = brCoord;

    // compute new center and size
    m_center = {(top+right)/2, (left+right)/2};
    auto width = QGeoCoordinate(m_center.latitude(), left).distanceTo(QGeoCoordinate(m_center.latitude(), right));
    auto height = QGeoCoordinate(bottom, m_center.longitude()).distanceTo(QGeoCoordinate(top, m_center.longitude()));
    m_size = {width, height};

    // rebuild ellipse shape
    updateEllipse();

    //
    emit centerChanged(m_center);
    emit sizeChanged(m_size);
}

const QSet<MapEllipseItem *> &MapEllipseItem::items()
{
    return m_items;
}

bool MapEllipseItem::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if(!m_editable)
        return false;
    auto ctrlPoint = watched == &m_firstCtrl ? &m_firstCtrl : &m_secondCtrl;
    switch (event->type()) {
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
    {
        // We should to compute center and size
        // and then update ellipse item rect
        auto firstCtrl = watched == &m_firstCtrl ? &m_firstCtrl : &m_secondCtrl;
        auto secondCtrl = firstCtrl == &m_firstCtrl ? &m_secondCtrl : &m_firstCtrl;
        // compute center
        auto centerPoint = (firstCtrl->pos() + secondCtrl->pos()) / 2;
        m_center = GraphicsMap::toCoordinate(centerPoint);
        // compute left right top bottom
        auto first = firstCtrl->pos();
        auto second = secondCtrl->pos();
        double left, right, top, bottom;
        if(first.x() <= second.x()) {
            left = first.x();
            right = second.x();
        } else {
            left = second.x();
            right = first.x();
        }
        if(first.y() <= second.y()) {
            bottom = first.y();
            top = second.y();
        } else {
            bottom = second.y();
            top = first.y();
        }

        QPointF leftPoint(left, centerPoint.y());
        QPointF rightPoint(right, centerPoint.y());
        QPointF topPoint(centerPoint.x(), top);
        QPointF bottomPoint(centerPoint.x(), bottom);
        m_size.setWidth(GraphicsMap::toCoordinate(leftPoint).distanceTo(GraphicsMap::toCoordinate(rightPoint)));
        m_size.setHeight(GraphicsMap::toCoordinate(topPoint).distanceTo(GraphicsMap::toCoordinate(bottomPoint)));
        auto topLeftPoint = QPointF(left, top);
        auto bottomRightPoint = QPointF(right, bottom);
        m_rectCtrl.setRect({topLeftPoint, bottomRightPoint});
        QGraphicsEllipseItem::setRect({topLeftPoint, bottomRightPoint});
        //
        emit centerChanged(m_center);
        emit sizeChanged(m_size);
        // compute
        break;
    }
    case QEvent::GraphicsSceneHoverEnter:
    {
        ctrlPoint->setBrush(Qt::white);
        ctrlPoint->setScale(1.2);
        break;
    }
    case QEvent::GraphicsSceneHoverLeave:
    {
        ctrlPoint->setBrush(Qt::lightGray);
        ctrlPoint->setScale(1);
        break;
    }
    default:
        break;
    }

    return false;
}

QVariant MapEllipseItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
   if(change != ItemSceneHasChanged)
       return QGraphicsEllipseItem::itemChange(change, value);

   m_firstCtrl.installSceneEventFilter(this);
   m_secondCtrl.installSceneEventFilter(this);
   return QGraphicsEllipseItem::itemChange(change, value);
}

/*!
 * \brief MapEllipseItem::updateEllipse
 * \details 更新圆形实际是根据左上和右下两个点进行重建，所以在其他Setter函数必须提前将两个计算好
 */
void MapEllipseItem::updateEllipse()
{
    // compute topleft point and bottomright point in scene
    auto topLeftPoint = GraphicsMap::toScene(m_topLeftCoord);
    auto bottomRightPoint = GraphicsMap::toScene(m_bottomRightCoord);
    // update ellipse outlook
    QGraphicsEllipseItem::setRect({topLeftPoint, bottomRightPoint});
    // update ellipse's contorl points
    m_firstCtrl.setPos(topLeftPoint);
    m_secondCtrl.setPos(bottomRightPoint);
    m_rectCtrl.setRect({topLeftPoint, bottomRightPoint});
}

void MapEllipseItem::updateEditable()
{
    auto pen = this->pen();
    pen.setWidth(0);
    pen.setColor(m_editable ? Qt::white : Qt::lightGray);
    setPen(pen);

    m_rectCtrl.setVisible(m_editable);
    m_firstCtrl.setVisible(m_editable);
    m_secondCtrl.setVisible(m_editable);
}
