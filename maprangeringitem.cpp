#include "maprangeringitem.h"
#include "graphicsmap.h"
#include "mapobjectitem.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QtMath>

void qt_graphicsItem_highlightSelected(QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option);

QSet<MapRangeRingItem*> MapRangeRingItem::m_items;

MapRangeRingItem::MapRangeRingItem() :
    m_cross(true),
    m_rotation(0),
    m_attachObj(nullptr)
{
    m_pen.setWidth(2);
    m_pen.setCosmetic(true);
    m_pen.setColor(QColor::fromRgb(118, 215, 196));
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

void MapRangeRingItem::setCrossVisible(bool visible)
{
    m_cross = visible;
}

void MapRangeRingItem::setCoordinate(const QGeoCoordinate &coord)
{
    if(m_coord == coord)
        return;
    m_coord = coord;
    updateBoundingRect();
    setPos(GraphicsMap::toScene(m_coord));
}

void MapRangeRingItem::setRotation(const qreal &degree)
{
    if(m_rotation == degree)
        return;
    m_rotation = degree;
    // 保持椭圆和方位刻度不变，然后在paint函数里面更新十字型和距离文字的位置
    update();
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
    if(m_attachObj) {
        disconnect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapRangeRingItem::setCoordinate);
        disconnect(m_attachObj, &MapObjectItem::rotationChanged, this, &MapRangeRingItem::setRotation);
    }
    //
    if(obj) {
        m_attachObj = obj;
        connect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapRangeRingItem::setCoordinate);
        connect(m_attachObj, &MapObjectItem::rotationChanged, this, &MapRangeRingItem::setRotation);
        setCoordinate(m_attachObj->coordinate());
        setRotation(m_attachObj->rotation());
    }
}

void MapRangeRingItem::detach()
{
    if(m_attachObj) {
        disconnect(m_attachObj, &MapObjectItem::coordinateChanged, this, &MapRangeRingItem::setCoordinate);
        disconnect(m_attachObj, &MapObjectItem::rotationChanged, this, &MapRangeRingItem::setRotation);
    }
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

    // radius of ellpise
    auto ellpiseRadius = [&](const float &radius) {
        auto topCoord = m_coord.atDistanceAndAzimuth(radius * 1e3, 0);
        auto rightCoord = m_coord.atDistanceAndAzimuth(radius * 1e3, 90);
        auto top = GraphicsMap::toScene(topCoord);
        auto right = GraphicsMap::toScene(rightCoord);
        auto center = this->pos();
        auto rx = right.rx() - center.rx();
        auto ry = center.ry() - top.ry();
        return QPointF(rx, ry);
    };
    //
    painter->setPen(m_pen);
    painter->setFont(m_font);
    painter->setBrush(Qt::NoBrush);
    auto radius0 = ellpiseRadius(m_radius / 3);
    auto radius1 = ellpiseRadius(m_radius / 3 * 2);
    auto radius2 = ellpiseRadius(m_radius);
    /*--------------- fixed ratation ----------------*/
    // 1.draw ellipse
    painter->drawEllipse({0, 0}, radius0.rx(), radius0.ry());
    painter->drawEllipse({0, 0}, radius1.rx(), radius1.ry());
    painter->drawEllipse({0, 0}, radius2.rx(), radius2.ry());

    // 2.draw dial with azimuth
    painter->save();
    QFontMetricsF fontMetrix(m_font);
    auto textRect = fontMetrix.boundingRect("N");
    for(int i = 0; i < 12; ++i) {
        int az = i * 30;
        QString number;
        switch (az) {
        case 30:
        case 60:
        case 120:
        case 150:
        case 210:
        case 240:
        case 300:
        case 330:
            number = QString::number(az);
            break;
        case 0:
            number = "N";
            break;
        case 90:
            number = "E";
            break;
        case 180:
            number = "S";
            break;
        case 270:
            number = "W";
            break;
        }
        textRect = fontMetrix.boundingRect(number);
        painter->drawLine(0.0, -radius2.ry(), 0.0, -radius2.ry()*0.98);
        painter->drawText(-textRect.width()/2, -radius2.y() + textRect.height(), number);
        painter->rotate(30);
    }
    painter->restore();

    /*--------------- mutable ratation ----------------*/
    // 3.draw cross shape with a ligter color
    // Painter Begin
    painter->save();
    painter->rotate(m_rotation);
    if(m_cross) {
        painter->save();
        auto pen = painter->pen();
        pen.setColor(pen.color().lighter());
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        painter->drawLine(-radius2.rx(), 0, radius2.rx(), 0);
        painter->drawLine(0 ,-radius2.ry(), 0, radius2.ry());
        painter->restore();
    }
    auto pen = painter->pen();
    pen.setColor(pen.color().lighter(130));
    painter->setPen(pen);

    // 4.draw distance text
    painter->rotate(45);
    auto km = QString::number(m_radius/3)+"Km";
    textRect = fontMetrix.boundingRect(km);
    painter->drawText(-textRect.width()/2, -radius0.y(), km);
    km = QString::number(m_radius/3*2)+"Km";
    textRect = fontMetrix.boundingRect(km);
    painter->drawText(-textRect.width()/2, -radius1.y(), km);
    km = QString::number(m_radius)+"Km";
    textRect = fontMetrix.boundingRect(km);
    painter->drawText(-textRect.width()/2, -radius2.y(), km);
    painter->rotate(-45);
    // Painter End
    painter->restore();

    // copied from qt source (maybe not work for current)
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
    auto rx = right.rx() - center.rx();
    auto ry = center.ry() - top.ry();
    m_boundRect.adjust(-rx, -ry, rx, ry);
    m_boundRect.adjust(-halfpw, -halfpw, halfpw, halfpw);
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
