#include "mapobjectitem.h"
#include "graphicsmap.h"
#include <QGraphicsColorizeEffect>

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
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
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

void MapObjectItem::setColor(const QColor &color, qreal strength)
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

void MapObjectItem::setRotation(const float &yaw)
{
    if(qFuzzyCompare(yaw, m_yaw))
        return;
    m_yaw = yaw;
    setRotation(m_yaw);
}

const QSet<MapObjectItem *> &MapObjectItem::items()
{
    return m_items;
}
