﻿#include "mapobjectitem.h"
#include "graphicsmap.h"
#include <QGraphicsColorizeEffect>
#include <QGraphicsSceneEvent>
#include <QDebug>

/* XPM */
static const char *default_xpm[] = {
/* columns rows colors chars-per-pixel */
"32 32 9 1 ",
"  c None",
". c #19F929",
"X c #1AF928",
"o c #1AF929",
"O c #19FA29",
"+ c #1AFA29",
"@ c #1BFA29",
"# c #1AFB29",
"$ c #1AFB2A",
/* pixels */
"                                ",
"                                ",
"               ++               ",
"              ++++              ",
"              ++++              ",
"              ++++              ",
"           ++++++++++           ",
"          ++++++++++++          ",
"        ++++++++++++++++        ",
"        +++++ ++++ +++++        ",
"       #++++  ++++  ++++#       ",
"      +++++    ++    +++++      ",
"      ++++            +++#      ",
"      +++              +++      ",
"   ++++++++          ++++++++   ",
"  ++++++++++        ++++++++++  ",
"  ++++++++++        ++++++++++  ",
"   +++++++++        +++++++++   ",
"      +++              +++      ",
"      ++++            ++++      ",
"      +++++   ++++   +++++      ",
"       +++++  ++++  +++++       ",
"        ++++O ++++ +++++        ",
"        ++++++++++++++++        ",
"          #+++++++++++          ",
"           +#++++++#+           ",
"              ++++              ",
"              ++++              ",
"              ++++              ",
"               ++               ",
"                                ",
"                                "
};

QSet<MapObjectItem*> MapObjectItem::m_items;

MapObjectItem::MapObjectItem(const QGeoCoordinate &coord)
{
    auto font = m_text.font();
    font.setFamily("Microsoft YaHei");
    font.setPointSize(10);
    m_text.setFont(font);
    m_text.setBrush(Qt::black);
    m_text.setParentItem(this);
    m_border.setPen(QPen(Qt::lightGray));
    m_border.setVisible(false);
    m_border.setParentItem(this);
    //
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setTransformationMode(Qt::SmoothTransformation);
    setIcon(QString());
    //
    m_items.insert(this);

    //
    setCoordinate(coord);
}

MapObjectItem::~MapObjectItem()
{
    m_items.remove(this);
}

void MapObjectItem::setCoordinate(const QGeoCoordinate &coord)
{
    if(m_coord == coord)
        return;

    m_coord = coord;
    this->setPos(GraphicsMap::toScene(coord));
    emit coordinateChanged(coord);
}

const QGeoCoordinate &MapObjectItem::coordinate() const
{
    return m_coord;
}

void MapObjectItem::setIcon(const QPixmap &pixmap)
{
    this->setOffset(0, 0);
    // Reset to default icon
    if(pixmap.isNull()) {
        this->setPixmap(QPixmap(default_xpm));
    }
    else {
        this->setPixmap(pixmap);
    }
    // make sure that the center of icon is positioned at current position
    auto boundRect = this->boundingRect();
    setOffset(-boundRect.center());
}

void MapObjectItem::setIconColor(const QColor &color, qreal strength)
{
    // We should to unset previous color
    if(!color.isValid()) {
        this->setGraphicsEffect(nullptr);
        return;
    }

    auto effect = dynamic_cast<QGraphicsColorizeEffect*>(graphicsEffect());
    if(!effect) {
        effect = new QGraphicsColorizeEffect;
        this->setGraphicsEffect(effect);
    }
    effect->setColor(color);
    effect->setStrength(strength);
}

void MapObjectItem::setText(const QString &text, Qt::Alignment align)
{
    m_text.setText(text);
    auto iconBound = this->boundingRect();
    auto textBound = m_text.boundingRect();
    QPointF pos(-textBound.center());
    if(align & Qt::AlignLeft) {
        pos.setX(-iconBound.width()/2 - textBound.center().x());
    }
    else if(align & Qt::AlignHCenter) {
        pos.setX(-textBound.center().x());
    }
    else if(align & Qt::AlignRight) {
        pos.setX(iconBound.width()/2);
    }
    if(align & Qt::AlignTop) {
        pos.setY(-iconBound.height()/2 - textBound.height());
    }
    else if(align & Qt::AlignVCenter) {
        pos.setY(-textBound.center().y());
    }
    else if(align & Qt::AlignBottom) {
        pos.setY(iconBound.height()/2);
    }
    m_text.setPos(pos);
}

void MapObjectItem::setTextColor(const QColor &color)
{
    m_text.setPen(color);
    m_text.setBrush(color);
}

void MapObjectItem::setAllowMouseEvent(bool enable)
{
    m_enableMouse = enable;
}

void MapObjectItem::setMoveable(bool movable)
{
    this->setFlag(QGraphicsItem::ItemIsMovable, movable);
    this->setAcceptHoverEvents(movable);
}

void MapObjectItem::setCheckable(bool checkable)
{
    // be sure that setChecked is called first
    // and then do operator=
    if(checkable == false)
        setChecked(false);
    m_checkable = checkable;
}

void MapObjectItem::setChecked(bool checked)
{
    if(!m_checkable)
        return;
    if(m_checked == checked)
        return;
    m_checked = checked;
    if(checked) {
        m_border.setRect(this->boundingRect());
        m_border.setVisible(true);
    }
    else {
        m_border.setVisible(false);
    }
}

void MapObjectItem::toggle()
{
    setChecked(!m_checked);
}

bool MapObjectItem::isChecked() const
{
    return m_checked;
}

void MapObjectItem::setRoute(MapRouteItem *route)
{
    if(m_route == route)
        return;
    m_route = route;
    emit routeChanged(route);
}

MapRouteItem *MapObjectItem::route() const
{
    return m_route;
}

void MapObjectItem::setSpeed(double speed)
{
    m_speed = speed;
}

const double MapObjectItem::getSpeed() const
{
    return m_speed;
}

const QSet<MapObjectItem *> &MapObjectItem::items()
{
    return m_items;
}

QVariant MapObjectItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
   if(change == ItemRotationHasChanged) {
       emit rotationChanged(this->rotation());
   }
   return QGraphicsPixmapItem::itemChange(change, value);
}

void MapObjectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsPixmapItem::hoverEnterEvent(event);
    if(this->flags() & QGraphicsItem::ItemIsMovable) {
        this->setScale(1.2);
        this->setCursor(Qt::DragMoveCursor);
    }
}

void MapObjectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsPixmapItem::hoverEnterEvent(event);
    if(this->flags() & QGraphicsItem::ItemIsMovable) {
        this->setScale(1.1);
        this->setCursor(Qt::ArrowCursor);
    }
}

void MapObjectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mousePressEvent(event);

    if(event->buttons() & Qt::RightButton){
        if(m_enableMouse)
            event->accept();
        emit menuRequest();
        return;
    }


    if(m_enableMouse)
        event->accept();
    // else will no longer propagate event to mouseMoveEvent and mouseReleaseEvent
    m_pressPos = event->screenPos();
    emit pressed();
}

void MapObjectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseMoveEvent(event);

    m_coord = GraphicsMap::toCoordinate(this->scenePos());
    emit coordinateDragged(m_coord);
}

void MapObjectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseReleaseEvent(event);
    // QGraphicsItem::ItemIsMovable will case the function be called
    if(!m_enableMouse)
        return;
    // if moved some distance, we ignore switch-check
    if(m_checkable && ((m_pressPos-event->screenPos()).manhattanLength() < 3)
            && this->contains(event->pos())) {
        setChecked(!m_checked);
        emit toggled(m_checked);
    }
    emit released();
    emit clicked(m_checked);
}

void MapObjectItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseDoubleClickEvent(event);
    emit doubleClicked();
}
