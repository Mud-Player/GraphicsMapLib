#include "maprangeringitem.h"
#include "graphicsmap.h"
#include "mapobjectitem.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QTimer>
#include <QDebug>

void qt_graphicsItem_highlightSelected(QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option);

QSet<MapRangeRingItem*> MapRangeRingItem::m_items;

MapRangeRingItem::MapRangeRingItem() :
    m_attachObj(nullptr)
{
    m_pen.setWidth(2);
    m_pen.setColor(Qt::green);
    m_pen.setCosmetic(true);
    m_pen.setColor(QColor::fromRgb(100, 190, 181));
    m_font.setFamily("Microsoft YaHei");
    m_font.setPointSizeF(10);
    setCoordinate({0, 0});
    setRadius(60);
    //
    m_items.insert(this);
}

MapRangeRingItem::~MapRangeRingItem()
{
    m_items.remove(this);
}

void MapRangeRingItem::setCoordinate(const QGeoCoordinate &coord)
{
    if(m_coord == coord)
        return;
    m_coord = coord;
    updateBoundingRect();
    setPos(GraphicsMap::toScene(m_coord));
}

void MapRangeRingItem::setRadius(const float &km)
{
    if(m_radius == km)
        return;
    m_radius = km;
    updateBoundingRect();
    // 30km respond to 10 point size for font
    m_font.setPointSizeF(km / 30 * 10);
}

void MapRangeRingItem::attach(MapObjectItem *obj)
{
    if(m_attachObj)
        disconnect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapRangeRingItem::setCoordinate);
    //
    m_attachObj = obj;
    if(m_attachObj)
        connect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapRangeRingItem::setCoordinate);
}

void MapRangeRingItem::detach()
{
    if(m_attachObj)
        disconnect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapRangeRingItem::setCoordinate);
    m_attachObj = nullptr;
}

void MapRangeRingItem::setPen(const QPen &pen)
{
    if(m_pen == pen)
        return;
    m_pen = pen;
    updateBoundingRect();
}

void MapRangeRingItem::setFont(const QFont &font)
{
    if(m_font == font)
        return;
    m_font = font;
    update();
}

QPen MapRangeRingItem::pen() const
{
    return m_pen;
}

QFont MapRangeRingItem::font() const
{
    return m_font;
}

const QSet<MapRangeRingItem *> &MapRangeRingItem::items()
{
    return m_items;
}

QRectF MapRangeRingItem::boundingRect() const
{
    return m_boundRect;
}

void MapRangeRingItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    painter->setPen(m_pen);
    painter->setFont(m_font);
    drawEllipse(painter, m_radius / 3);
    drawEllipse(painter, m_radius / 3 * 2);
    drawEllipse(painter, m_radius);
    // copied from qt source
    if (option->state & QStyle::State_Selected)
        qt_graphicsItem_highlightSelected(this, painter, option);
}

void MapRangeRingItem::updateBoundingRect()
{
    prepareGeometryChange();
    qreal halfpw = m_pen.style() == Qt::NoPen ? qreal(0) : m_pen.widthF() / 2;
    auto topCoord = m_coord.atDistanceAndAzimuth(m_radius * 1e3, 0);
    auto rightCoord = m_coord.atDistanceAndAzimuth(m_radius * 1e3, 90);
    auto top = GraphicsMap::toScene(topCoord);
    auto right = GraphicsMap::toScene(rightCoord);
    auto center = this->pos();
    auto rx = right.rx() - center.x();
    auto ry = center.y() - top.ry();
    m_boundRect.adjust(-rx, -ry, rx, ry);
    m_boundRect.adjust(-halfpw, -halfpw, halfpw, halfpw);
}

void MapRangeRingItem::drawEllipse(QPainter *painter, const float &radius)
{
    auto topCoord = m_coord.atDistanceAndAzimuth(radius * 1e3, 0);
    auto rightCoord = m_coord.atDistanceAndAzimuth(radius * 1e3, 90);
    auto top = GraphicsMap::toScene(topCoord);
    auto right = GraphicsMap::toScene(rightCoord);
    auto center = this->pos();
    auto rx = right.rx()-center.rx();
    auto ry = center.ry()-top.ry();
    // draw the ellpise
    painter->drawEllipse({0, 0}, rx, ry);
    // draw dial
    if(m_radius == radius) {
        painter->save();
        auto pen = painter->pen();
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        painter->drawLine(0, ry, 0, -ry);
        painter->drawLine(-rx, 0, rx, 0);
        painter->restore();
    }
    // draw rang text
    auto textCoord = m_coord.atDistanceAndAzimuth(radius * 1e3, 45);
    auto textPos = GraphicsMap::toScene(textCoord);
    painter->drawText(textPos-center, QString::number(radius, 10, 1)+"Km");
}

void qt_graphicsItem_highlightSelected(QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
    if (qFuzzyIsNull(qMax(murect.width(), murect.height())))
        return;

    const QRectF mbrect = painter->transform().mapRect(item->boundingRect());
    if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
        return;

    qreal itemPenWidth = 1;
    const qreal pad = itemPenWidth / 2;

    const qreal penWidth = 0; // cosmetic pen

    const QColor fgcolor = option->palette.windowText().color();
    const QColor bgcolor( // ensure good contrast against fgcolor
        fgcolor.red()   > 127 ? 0 : 255,
        fgcolor.green() > 127 ? 0 : 255,
        fgcolor.blue()  > 127 ? 0 : 255);

    painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));

    painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));
}
