#include "maplabelitem.h"
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QDebug>

MapLabelItem::MapLabelItem()
{
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    //
    auto font = m_title.font();
    auto brush = m_title.brush();
    font.setFamily("Microsoft YaHei");
    font.setPointSizeF(16);
    brush.setColor(Qt::white);
    m_title.setFont(font);
    m_title.setBrush(brush);
    m_title.setParentItem(this);
    //
    font.setPointSizeF(12);
    m_text.setFont(font);
    m_text.setBrush(brush);
    m_text.setParentItem(this);
}

void MapLabelItem::setBackground(const QPixmap &pixmap)
{
    this->setOffset(0, 0);
    this->setPixmap(pixmap);
    this->setOffset(-this->boundingRect().center());
    updateLayout();
}

void MapLabelItem::setTitle(const QString &title)
{
    bool move = m_text.boundingRect().isNull() && !this->pixmap().isNull();
    m_title.setText(title);
    if(move)
        updateLayout();
}

void MapLabelItem::setText(const QString &text)
{
    bool move = m_text.boundingRect().isNull() && !this->pixmap().isNull();
    m_text.setText(text);
    if(move)
        updateLayout();
}

void MapLabelItem::updateLayout()
{
    auto parentBound = this->boundingRect();
    auto titleBound = m_title.boundingRect();
    auto textBound = m_text.boundingRect();
    auto spacing = (parentBound.height() - titleBound.height() - textBound.height()) / 3;
    //
    auto x = - titleBound.width() / 2;
    auto y = - (parentBound.height()/2 - spacing);
    m_title.setPos(x, y);
    //
    x = - textBound.width() / 2;
    y = - (parentBound.height()/2 - spacing - textBound.height());
    m_text.setPos(x, y);
}
