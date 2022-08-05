#include "maptableitem.h"
#include "graphicsmap.h"
#include <QPainter>
#include <math.h>
#include <QGraphicsSceneEvent>
#include <QDebug>
#define M_PI 3.14159265358979323846


MapTableItem::MapTableItem(const QGeoCoordinate &coord, QGraphicsItem * parent)
    :QGraphicsItem(parent)
{
    m_FixedAngle = 0;
    memset(m_Margin, 0x00, sizeof(int)* _upper);

    m_nMaxWide = 0;
    m_nCurHigh = 0;
    m_BackWide = 0;
    m_RoundRadius = 0;
    m_bFixedDirect = 0;

    m_ScutSize = QSize(6.0, 6.0);

    m_BorderPen = QPen(QColor(128, 255, 255, 200));
    m_BackBrush = QBrush(QColor(128, 255, 255, 50));
    m_backPixmap = QPixmap(":/Resources/logo.png");

    m_AnchorPos = AP_TOPLEFT;

    QPen pen(Qt::DotLine);
    pen.setColor(Qt::lightGray);
    m_border.setPen(pen);
    m_border.setVisible(false);
    m_border.setParentItem(this);

    setMargins(5, 5, 5, 5);
    setSpacing(5);

    this->setMoveable(true);
    this->setAllowMouseEvent(true);
    this->setCheckable(true);

    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    this->setPos(GraphicsMap::toScene(coord));
}

MapTableItem::~MapTableItem()
{

}

void MapTableItem::setAnchorPosition(AnchorPosition anchorPos)
{
    m_AnchorPos = anchorPos;
}

void MapTableItem::setMargins(int left, int top, int right, int bottom)
{
    m_Margin[_left] = left;
    m_Margin[_top] = top;
    m_Margin[_right] = right;
    m_Margin[_bottom] = bottom;
}

void MapTableItem::setSpacing(int space)
{
    m_Margin[_space] = space;
}

void MapTableItem::addField(const QString &field, bool bVolatile)
{
    if(m_Infoes.contains(field)){
        m_Infoes[field]->bVolatile = bVolatile;
        return;
    }

    m_Fields.push_back(field);

    FieldInfo *info = new FieldInfo(bVolatile);
    m_Infoes.insert(field, info);
}

void MapTableItem::inrField(int pos, const QString &field, bool bVolatile)
{
    if(pos >= m_Fields.count())
        m_Fields.push_back(field);
    else
        m_Fields.insert(pos, field);

    FieldInfo *info = new FieldInfo(bVolatile);
    m_Infoes.insert(field, info);
}

void MapTableItem::delField(const QString &field)
{
    m_Fields.takeAt(m_Fields.indexOf(field));
    auto item = m_Infoes.find(field);
    m_Infoes.erase(item);
}

void MapTableItem::setFieldFont(const QString &field, const QFont &font)
{
    if(m_Infoes.contains(field)){
        m_Infoes[field]->fieldFont = font;
    }
}

void MapTableItem::setValueFont(const QString &field, const QFont &font)
{
    if(m_Infoes.contains(field)){
        m_Infoes[field]->valueFont = font;
    }
}

void MapTableItem::setFieldPen(const QString &field, const QPen &pen)
{
    if(m_Infoes.contains(field)){
        m_Infoes[field]->fieldPen = pen;
    }
}

void MapTableItem::setValuePen(const QString &field, const QPen &pen)
{
    if(m_Infoes.contains(field)){
        m_Infoes[field]->valuePen = pen;
    }
}

void MapTableItem::setValue(const QString &field, const QString &value)
{        
    if(m_Infoes.contains(field)){
        m_Infoes[field]->value = value;
    }
}

void MapTableItem::setFixDirect(bool bFixDirect, double fixedangle)
{
    m_bFixedDirect = bFixDirect;
}

void MapTableItem::setRoundedRadius(int roundRadius)
{
    m_RoundRadius = roundRadius;
}

void MapTableItem::setBorderPen(const QPen &borderPen)
{
    m_BorderPen = borderPen;
}

void MapTableItem::setBackBrush(const QBrush &brush)
{
    m_BackBrush = brush;
}

void MapTableItem::setBackPixmap(const QPixmap &pixmap)
{
    m_backPixmap = pixmap;
}

QRectF MapTableItem::tabletRect()
{
    auto rect = boundingRect();
    return rect;
}

QString MapTableItem::getValue(const QString &field)
{
    if(m_Infoes.contains(field)){
        return m_Infoes[field]->value;
    }
    return QString();
}

void MapTableItem::updateTableSize()
{
    UpdateInfo();
}

void MapTableItem::setAllowMouseEvent(bool enable)
{
    m_enableMouse = enable;
}

void MapTableItem::setMoveable(bool movable)
{
    this->setFlag(QGraphicsItem::ItemIsMovable, movable);
    this->setAcceptHoverEvents(movable);
}

void MapTableItem::setCheckable(bool checkable)
{
    if(checkable == false)
        setChecked(false);
    m_checkable = checkable;
}

void MapTableItem::setChecked(bool checked)
{
    if(!m_checkable)
        return;
    if(m_checked == checked)
        return;
    m_checked = checked;
    if(checked) {
        auto rect = this->boundingRect();
        auto leftTop = rect.topLeft();
        auto rightBottom = rect.bottomRight();

        leftTop += QPoint(-5, -5);
        rightBottom += QPoint(5, 5);
        m_border.setRect(QRectF(leftTop, rightBottom));
        m_border.setVisible(true);
    }
    else {
        m_border.setVisible(false);
    }
}

void MapTableItem::toggle()
{
    setChecked(!m_checked);
}

bool MapTableItem::isChecked() const
{
    return m_checked;
}

void MapTableItem::UpdateInfo()
{
    int nWide = m_Margin[_left] + m_Margin[_right] + m_Margin[_space];
    int nHigh = m_Margin[_top] + m_Margin[_bottom] + m_Margin[_space] * (m_Fields.size() - 1);

    static int maxFieldWidth = 0;
    static int maxValueWidth = 0;

    for (auto &field : qAsConst(m_Fields))
    {
        auto curFieldFont = m_Infoes[field]->fieldFont;
        auto curValueFont = m_Infoes[field]->valueFont;
        auto curValue = m_Infoes[field]->value;

        QFontMetrics fieldMet(curFieldFont);
        QFontMetrics valueMet(curValueFont);

        QRect fieldRect = fieldMet.boundingRect(field);
        QRect valueRect = valueMet.boundingRect(curValue);

        int nFieldWide = fieldRect.width();
        int nValueWide = valueRect.width();
        int nFieldHigh = fieldRect.height();
        int nValueHigh = valueRect.height();

        maxFieldWidth  = maxFieldWidth > nFieldWide ? maxFieldWidth : nFieldWide;
        maxValueWidth  = maxValueWidth > nValueWide ? maxValueWidth : nValueWide;

        nHigh   += nFieldHigh > nValueHigh ? nFieldHigh : nValueHigh;
        m_Margin[_maxField] = m_Margin[_maxField] > nFieldWide ? m_Margin[_maxField] : nFieldWide;
    }
    m_nMaxWide = maxFieldWidth + maxValueWidth;
    m_ScutSize.setWidth(nWide + m_nMaxWide);
    m_ScutSize.setHeight(nHigh);
}

QRectF MapTableItem::boundingRect() const
{
    int nScutWide = m_ScutSize.width();
    int nScutHigh = m_ScutSize.height();
    QRectF rect(QPointF(0.0, 0.0), m_ScutSize);
    switch (m_AnchorPos)
    {
    case AP_TOPLEFT:    break;
    case AP_TOPRIGHT:   rect.setTopLeft(QPointF(-nScutWide, 0.0)); break;
    case AP_BOTTOMLEFT: rect.setTopLeft(QPointF(0.0, -nScutHigh)); break;
    case AP_BOTTOMRIGHT:rect.setTopLeft(QPointF(-nScutWide, -nScutHigh));break;
    case AP_MIDDLELEFT: rect.setTopLeft(QPointF(0.0, -nScutHigh / 2)); break;
    case AP_MIDDLERIGHT:rect.setTopLeft(QPointF(-nScutWide , -nScutHigh / 2)); break;
    case AP_MIDDLETOP:  rect.setTopLeft(QPointF(-nScutWide / 2, 0.0)); break;
    case AP_MIDDLEBTTOM:rect.setTopLeft(QPointF(-nScutWide / 2, -nScutHigh ));break;
    case AP_CENTER:     rect.setTopLeft(QPointF(-nScutWide / 2, -nScutHigh / 2));break;
    default:            break;
    }

    return rect;
}

QPainterPath MapTableItem::shape() const
{
    QPainterPath path;
    QRectF rect = boundingRect();
    QPointF topleft  = rect.topLeft();
    QPointF btmright = rect.bottomRight();
    QRectF curRect(topleft, btmright);
    path.addRect(curRect);

    return path;
}

void MapTableItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF  rect = boundingRect();
    QPointF otopleft = rect.topLeft();
    QPointF obtmrigt = rect.bottomRight();
    QRectF curRect(otopleft, obtmrigt);

    QPointF topleft = curRect.topLeft();
    QPointF topright = curRect.topRight();
    QPointF btmleft = curRect.bottomLeft();
    QPointF btmright = curRect.bottomRight();

    painter->save();
    painter->rotate(rotation());

    painter->drawImage(rect, m_backPixmap.toImage());
    switch (m_AnchorPos)
    {
    case AP_TOPLEFT:
    case AP_TOPRIGHT:
    case AP_BOTTOMLEFT:
    case AP_BOTTOMRIGHT:
    case AP_CENTER:
    {
        QPen curPen(m_BorderPen);
        curPen.setWidthF(m_BorderPen.widthF());
        painter->setPen(curPen);
        painter->setBrush(m_BackBrush);
        painter->drawRect(curRect);
        break;
    }
    case AP_MIDDLELEFT:
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_BackBrush);

        QPen curPen(m_BorderPen);
        curPen.setWidthF(m_BorderPen.widthF());

        QPainterPath path;
        path.moveTo(QPointF(0.0, 0.0));
        path.lineTo(btmleft);
        path.lineTo(btmright);
        path.lineTo(topright);
        path.lineTo(topleft);
        path.lineTo(QPointF(0.0, 0.0));
        painter->setPen(curPen);
        painter->drawPath(path);
        break;
    }
    case AP_MIDDLERIGHT:
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_BackBrush);

        QPen curPen(m_BorderPen);
        curPen.setWidthF(m_BorderPen.widthF());

        QPainterPath path;
        path.moveTo(QPointF(0.0, 0.0));
        path.lineTo(btmright);
        path.lineTo(btmleft);
        path.lineTo(topleft);
        path.lineTo(topright);
        path.lineTo(QPointF(0.0, 0.0));
        painter->setPen(curPen);
        painter->drawPath(path);
        break;
    }
    case AP_MIDDLETOP:
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_BackBrush);

        QPen curPen(m_BorderPen);
        curPen.setWidthF(m_BorderPen.widthF());

        QPainterPath path;
        path.moveTo(QPointF(0.0, 0.0));
        path.lineTo(topleft);
        path.lineTo(btmleft);
        path.lineTo(btmright);
        path.lineTo(topright);
        path.lineTo(QPointF(0.0, 0.0));
        painter->setPen(curPen);
        painter->drawPath(path);
        break;
    }
    case AP_MIDDLEBTTOM:
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_BackBrush);

        QPen curPen(m_BorderPen);
        curPen.setWidthF(m_BorderPen.widthF());

        QPainterPath path;
        path.moveTo(QPointF(0.0, 0.0));
        path.lineTo(btmright);
        path.lineTo(topright);
        path.lineTo(topleft);
        path.lineTo(btmleft);
        path.lineTo(QPointF(0.0, 0.0));
        painter->setPen(curPen);
        painter->drawPath(path);
        break;
    }
    default:
        break;
    }

    int nHighOffset = m_Margin[_top] - 1.0;
    for(auto &field : qAsConst(m_Fields))
    {
        QFont fieldFont = m_Infoes[field]->fieldFont;
        QPen fieldPen = m_Infoes[field]->fieldPen;
        QFontMetrics fieldMet(fieldFont);

        QFont curFont = m_Infoes[field]->valueFont;
        QString curValue = m_Infoes[field]->value;
        QPen curPen = m_Infoes[field]->valuePen;

        QFontMetrics valueMet(curFont);
        QRect fieldRect = fieldMet.boundingRect(field);
        QRect valueRect = valueMet.boundingRect(curValue);

        int nFieldHigh = fieldRect.height();
        int nValueHigh = valueRect.height();
        int nMortValue = nFieldHigh > nValueHigh ? nFieldHigh : nValueHigh;
        int yOffsetVal = nHighOffset + nMortValue;

        QPointF fieldPos(m_Margin[_left], yOffsetVal);
        QPointF valuePos(m_Margin[_left] + m_Margin[_space] + m_Margin[_maxField] , yOffsetVal);
        fieldPos = rect.topLeft() + fieldPos;
        valuePos = rect.topLeft() + valuePos;

        painter->setRenderHint(QPainter::TextAntialiasing, true);

        painter->setFont(fieldFont);
        painter->setPen(fieldPen);
        painter->drawText(fieldPos, field);
        painter->setFont(curFont);
        painter->setPen(curPen);
        painter->drawText(valuePos, curValue);
        nHighOffset += (nMortValue + m_Margin[_space] - 1.0);
    }

    painter->restore();
}

QVariant MapTableItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

void MapTableItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverEnterEvent(event);
    if(this->flags() & QGraphicsItem::ItemIsMovable) {
        this->setScale(1.2);
        this->setCursor(Qt::DragMoveCursor);
    }
}

void MapTableItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverLeaveEvent(event);
    if(this->flags() & QGraphicsItem::ItemIsMovable) {
        this->setScale(1.1);
        this->setCursor(Qt::ArrowCursor);
    }
}

void MapTableItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);

    if(m_enableMouse)
        event->accept();
    else
        event->ignore();
    m_pressPos = event->screenPos();
}

void MapTableItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);

    /// 由于该Item 可能继承其他 Item  所以在此传递其Item 在场景里的坐标
    emit coordinateDragged(this->scenePos());
}

void MapTableItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    if(!m_enableMouse)
        return;
    // if moved some distance, we ignore switch-check
    if(m_checkable && ((m_pressPos-event->screenPos()).manhattanLength() < 3)
            && this->contains(event->pos())) {
        setChecked(!m_checked);
    }
}

void MapTableItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
    emit doubleClicked();
}

MapTableItem::FieldInfo::FieldInfo()
{
    fieldHigh = 6.0;
    bVolatile = false;
    fieldFont = QFont("Microsoft YaHei", 10);
    valueFont = QFont("Microsoft YaHei", 10);
    fieldPen = QPen(QColor(128, 255, 255, 200));
    valuePen = QPen(QColor(128, 255, 255, 200));
    valuePos = {0, 0};
    value = " ";
}

MapTableItem::FieldInfo::FieldInfo(bool _bVolatile)
{
    fieldHigh = 6.0;
    bVolatile = _bVolatile;
    fieldFont = QFont("Microsoft YaHei", 10);
    valueFont = QFont("Microsoft YaHei", 10);
    fieldPen = QPen(QColor(128, 255, 255, 200));
    valuePen = QPen(QColor(128, 255, 255, 200));
    valuePos = {0, 0};
    value = " ";
}

MapTableItem::FieldInfo::FieldInfo(const FieldInfo &ot)
{
    fieldHigh = ot.fieldHigh;
    bVolatile = ot.bVolatile;
    fieldFont = ot.fieldFont;
    valueFont = ot.valueFont;
    fieldPen = ot.fieldPen;
    valuePen = ot.valuePen;
    valuePos = ot.valuePos;
    value = ot.value;
}
