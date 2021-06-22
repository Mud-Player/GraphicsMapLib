#include "mapobjectitem.h"
#include "graphicsmap.h"
#include <QGraphicsColorizeEffect>
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

MapObjectItem::MapObjectItem()
{
    auto font = m_text.font();
    font.setFamily("Microsoft YaHei");
    m_text.setFont(font);
    m_text.setBrush(Qt::black);
    m_text.setParentItem(this);
    //
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setTransformationMode(Qt::SmoothTransformation);
    setIcon(QString());
    //
    m_items.insert(this);
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

void MapObjectItem::setIcon(const QString &url)
{
    this->setPixmap(url);
    // Reset to default icon
    if(pixmap().isNull()) {
        this->setPixmap(QPixmap(default_xpm));
    }
    // make sure that the center of icon is positioned at current position
    auto boundRect = this->boundingRect();
    setOffset(-boundRect.center());
}

void MapObjectItem::setIconColor(const QColor &color, qreal strength)
{
    // We should to unset previous color
    if(!color.isValid()) {
        if(graphicsEffect())
            delete graphicsEffect();
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
    QPointF pos;
    if(align & Qt::AlignLeft) {
        pos.setX(-iconBound.center().x() - textBound.x());
    }
    else if(align & Qt::AlignHCenter) {
        pos.setX(-textBound.center().x());
    }
    else if(align % Qt::AlignRight) {
        pos.setX(iconBound.center().x());
    }
    if(align & Qt::AlignTop) {
        pos.setY(-iconBound.center().y());
    }
    else if(align & Qt::AlignVCenter) {
        pos.setY(-iconBound.center().y() - textBound.y());
    }
    else if(align & Qt::AlignBottom) {
        pos.setY(iconBound.center().y());
    }
    m_text.setPos(pos);
}

void MapObjectItem::setTextColor(const QColor &color)
{

}

void MapObjectItem::setMovable(bool movable)
{

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
   else if(change == ItemPositionHasChanged) {
       m_coord = GraphicsMap::toCoordinate(value.toPointF());
       emit coordinateChanged(m_coord);
   }
   return QGraphicsPixmapItem::itemChange(change, value);
}
