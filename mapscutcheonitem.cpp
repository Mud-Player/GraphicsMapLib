#include "mapscutcheonitem.h"
#include "graphicsmap.h"
#include "mapobjectitem.h"
#include "maptableitem.h"
#include <QPainter>
#include <math.h>
#include <QDebug>
#define M_PI 3.14159265358979323846


void MapSuctcheonItem::MapScutcheonLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(pen());
    painter->drawLine(line());
    painter->restore();
}

MapSuctcheonItem::MapSuctcheonItem(MapObjectItem *parent)
    :QGraphicsItem(parent)
{
    m_JoinPen = QPen(QColor(128, 255, 255, 200));
    m_JoinPen.setWidthF(1);
    m_pJoinLine = new MapScutcheonLine(this);
    if(m_pJoinLine){
        m_pJoinLine->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        m_pJoinLine->setPen(m_JoinPen);
    }

    m_pTablet = new MapTableItem({0, 0, 0}, m_pJoinLine);
    m_pTablet->setFixDirect(true, 45);
    setOffset(100, 45);

    connect(m_pTablet, &MapTableItem::coordinateDragged, this, &MapSuctcheonItem::onCoordinateDragged);

    m_border.setPen(QPen(Qt::lightGray));
    m_border.setVisible(false);
    m_border.setParentItem(this);
}

MapSuctcheonItem::~MapSuctcheonItem()
{

}

void MapSuctcheonItem::setMoveable(bool movable)
{
    this->setFlag(QGraphicsItem::ItemIsMovable, movable);
    this->setAcceptHoverEvents(movable);
}

void MapSuctcheonItem::setCheckable(bool checkable)
{
    // be sure that setChecked is called first
    // and then do operator=
    if(checkable == false)
        setChecked(false);
    m_checkable = checkable;
}

void MapSuctcheonItem::setChecked(bool checked)
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

void MapSuctcheonItem::toggle()
{
    setChecked(!m_checked);
}

bool MapSuctcheonItem::isChecked() const
{
    return m_checked;
}

void MapSuctcheonItem::setMargins(int left, int top, int right, int bottom)
{
    m_pTablet->setMargins(left, top, right, bottom);
}

void MapSuctcheonItem::setSpacing(int space)
{
    m_pTablet->setSpacing(space);
}

void MapSuctcheonItem::setOffset(qreal joinLineLength, int fixedAngle)
{
    QPointF endPos;
    auto sinAngle = sin(fixedAngle * M_PI / 180);
    auto cosAngle = cos(fixedAngle * M_PI / 180);

    endPos = {sinAngle * joinLineLength, -cosAngle * joinLineLength};
    m_pJoinLine->setLine({QPointF(0 ,0), endPos});
    m_pTablet->setPos(m_pJoinLine->line().p2());
}

void MapSuctcheonItem::setOffset(QPointF endPos)
{
    m_pJoinLine->setLine({QPointF(0 ,0), endPos});
}

void MapSuctcheonItem::addField(const QString &field, bool bVolatile)
{
    m_pTablet->addField(field, bVolatile);
}

void MapSuctcheonItem::inrFiled(int pos, const QString &field, bool bVolatile)
{
    m_pTablet->inrField(pos, field, bVolatile);
}

void MapSuctcheonItem::delField(const QString &field)
{
    m_pTablet->delField(field);
}

void MapSuctcheonItem::setFieldFont(const QString &field, const QFont &font)
{
    m_pTablet->setFieldFont(field, font);
}

void MapSuctcheonItem::setValueFont(const QString &field, const QFont &font)
{
    m_pTablet->setValueFont(field, font);
}

void MapSuctcheonItem::setFieldPen(const QString &field, const QPen &pen)
{
    m_pTablet->setFieldPen(field, pen);
}

void MapSuctcheonItem::setValuePen(const QString &field, const QPen &pen)
{
    m_pTablet->setValuePen(field, pen);
}

void MapSuctcheonItem::setValue(const QString &field, const QString &value)
{
    m_pTablet->setValue(field, value);
}

void MapSuctcheonItem::setFixedDirect(bool bFiexdDirect, qreal fixedangle)
{
    m_pTablet->setFixDirect(bFiexdDirect, fixedangle);
}

void MapSuctcheonItem::setBorderPen(const QPen &borderPen)
{
    m_pTablet->setBorderPen(borderPen);
}

void MapSuctcheonItem::setBackBrush(const QBrush &brush)
{
    m_pTablet->setBackBrush(brush);
}

void MapSuctcheonItem::setJoinPen(const QPen &joinPen)
{
    m_JoinPen = joinPen;
}

void MapSuctcheonItem::updateTableSize()
{
    m_pTablet->updateTableSize();
}

QPainterPath MapSuctcheonItem::shape() const
{
    auto startPos = m_pJoinLine->line().p1();
    auto endPos = m_pJoinLine->line().p2();
    QPainterPath path(startPos);
    path.lineTo(endPos);

    QRectF rect = m_pTablet->tabletRect();
    QPointF topleft  = rect.topLeft() + endPos;
    QPointF btmright = rect.bottomRight() + endPos;
    QRectF curRect(topleft, btmright);
    path.addRect(curRect);

    return path;
}

QRectF MapSuctcheonItem::boundingRect() const
{
    auto line = m_pJoinLine->line();
    auto table = m_pTablet->tabletRect();

    auto width = abs(line.p2().x()) + table.width();
    auto height = abs(line.p2().y()) + table.height();
    return QRectF(-width, -height, width, height);
}

void MapSuctcheonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (m_pJoinLine && m_pTablet)
    {
        auto line = m_pJoinLine->line();
//        qDebug()<<line.p2() << "paint";
        //注释代码即为图表不给父对象时
//        // 拿到飞机在场景的坐标，转到当前视口坐标，加上线段的偏移量得到新的视口坐标，将新的视口坐标转到场景坐标上，将图表放在该场景新的坐标
//        auto item = (MapObjectItem *)this->parentItem();
//        auto sceenPos = item->pos();
//        auto viewPos = this->scene()->views().at(0)->mapFromScene(sceenPos);

//        QPoint sceenOffsetPos = viewPos + QPoint(line.x2(), line.y2());
//        ((QGraphicsItem*)m_pTablet)->setPos(this->scene()->views().at(0)->mapToScene(sceenOffsetPos));

        //m_pTablet->setPos(line.p2());
    }
}

void MapSuctcheonItem::onCoordinateDragged(const QPointF& point)
{
    if(m_pJoinLine){
        // 俩种亦可  第一种拿当前俩个的场景坐标转到屏幕坐标算线段的差值 赋给线段长度  第一种需要关联到事件悬停 Item scale.
        //          第二种直接拿图表的位置赋给线段长度
        auto startPos = m_pJoinLine->line().p1();
        /* first. Map element stretching is not considered here
        auto viewPos = this->scene()->views().at(0)->mapFromScene(point);
        auto lineViewPos = this->scene()->views().at(0)->mapFromScene(m_pJoinLine->scenePos());
        m_pJoinLine->setLine({startPos, viewPos - lineViewPos});
        */

        auto tableTopLeft = m_pTablet->pos();
        m_pJoinLine->setLine({startPos, tableTopLeft});
    }
}
