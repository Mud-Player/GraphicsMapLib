#include "maplineitem.h"
#include "graphicsmap.h"
#include <QDebug>

QSet<MapLineItem*> MapLineItem::m_items;

MapLineItem::MapLineItem()
{
    //
    m_items.insert(this);

    //
    auto pen = this->pen();
	pen.setWidth(2);
	pen.setCosmetic(true);  // it will be always 2 pixmap whatever scale transform
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	pen.setColor(Qt::white);
	this->setPen(pen);
    //
	auto font = m_text.font();
	font.setPointSizeF(14);
	font.setFamily("Microsoft YaHei");
	m_text.setFont(font);
	m_text.setBrush(Qt::yellow);
	m_text.setPen(QColor(0, 255, 0));
	m_text.setParentItem(this);
	m_text.setFlag(ItemIgnoresTransformations);
    //
    m_startIcon.setParentItem(this);
    m_startIcon.setFlag(ItemIgnoresTransformations);
    m_endIcon.setParentItem(this);
    m_endIcon.setFlag(ItemIgnoresTransformations);
}

MapLineItem::~MapLineItem()
{
	m_items.remove(this);
}

void MapLineItem::setStartPoint(const QGeoCoordinate & pt)
{
    if(m_endings.first == pt)
        return;
    m_endings.first = pt;

    if(!m_endings.second.isValid())
        m_endings.second = pt;

    updateEndings();
}

void MapLineItem::setEndPoint(const QGeoCoordinate & pt)
{
    if(m_endings.second == pt)
        return;
    m_endings.second = pt;

    if(!m_endings.first.isValid())
        m_endings.first = pt;

    updateEndings();
}


void MapLineItem::setText(const QString & text, Qt::Alignment align)
{
	m_text.setText(text);
	auto lineBound = this->boundingRect();
	auto textBound = m_text.boundingRect();
	QPointF pos(-textBound.center());
	if (align & Qt::AlignLeft) {
		pos.setX(-lineBound.width()/2);
	}
	else if (align & Qt::AlignHCenter) {
		pos.setX(0);
	}
	else if (align & Qt::AlignRight) {
		pos.setX(lineBound.width() / 2);
	}
	if (align & Qt::AlignTop) {
		pos.setY(- lineBound.height() / 2);
	}
	else if (align & Qt::AlignVCenter) {
		pos.setY(0);
	}
	else if (align & Qt::AlignBottom) {
		pos.setY(lineBound.height() / 2);
	}
	m_text.setPos(pos);
}

void MapLineItem::setTextColor(const QColor & color)
{
	m_text.setPen(color);
}

void MapLineItem::setFontSizeF(const qreal size)
{
	auto font = m_text.font();
	font.setPointSizeF(size);
	m_text.setFont(font);
}

void MapLineItem::setLineWidth(const int width)
{
	auto pen = this->pen();
	pen.setWidth(width);
	this->setPen(pen);
}

void MapLineItem::setStartIcon(const QPixmap &pixmap, Qt::Alignment align)
{
    m_startIcon.setPixmap(pixmap);
	auto boundRect = m_startIcon.boundingRect();
	QPointF offset(-boundRect.width() / 2, -boundRect.height());

	if (align & Qt::AlignLeft) {
		offset.setX(-boundRect.width());
	}
	else if (align & Qt::AlignHCenter) {
		offset.setX(-boundRect.width() / 2);
	}
	else if (align & Qt::AlignRight) {
		offset.setX(0);
	}
	if (align & Qt::AlignTop) {
		offset.setY(-boundRect.height());
	}
	else if (align & Qt::AlignVCenter) {
		offset.setY(0);
	}
	else if (align & Qt::AlignBottom) {
		offset.setY(boundRect.height());
	}
	m_startIcon.setOffset(offset);
    updateEndings();
}

void MapLineItem::setEndIcon(const QPixmap &pixmap, Qt::Alignment align)
{
    m_endIcon.setPixmap(pixmap);
	auto boundRect = m_endIcon.boundingRect();
	QPointF offset(-boundRect.width() / 2, -boundRect.height());

	if (align & Qt::AlignLeft) {
		offset.setX(-boundRect.width());
	}
	else if (align & Qt::AlignHCenter) {
		offset.setX(-boundRect.width() / 2);
	}
	else if (align & Qt::AlignRight) {
		offset.setX(boundRect.width() / 2);
	}
	if (align & Qt::AlignTop) {
		offset.setY(-boundRect.height());
	}
	else if (align & Qt::AlignVCenter) {
		offset.setY(0);
	}
	else if (align & Qt::AlignBottom) {
		offset.setY(boundRect.height());
	}
	m_endIcon.setOffset(offset);
    updateEndings();
}

const QPair<QGeoCoordinate, QGeoCoordinate> &MapLineItem::endings()
{
    return m_endings;
}

void MapLineItem::attach(MapObjectItem *obj, MapLabelItem *label)
{

}

const QSet<MapLineItem *> &MapLineItem::items()
{
    return m_items;
}

void MapLineItem::updateEndings()
{
    auto ending0 = GraphicsMap::toScene(m_endings.first);
    auto ending1 = GraphicsMap::toScene(m_endings.second);
    auto origin = (ending0 + ending1) / 2;
    auto p0 = ending0 - origin;
    auto p1 = ending1 - origin;
    this->setPos(origin);
    this->setLine({p0, p1});
    //
    m_startIcon.setPos(p0);
    m_endIcon.setPos(p1);
}
