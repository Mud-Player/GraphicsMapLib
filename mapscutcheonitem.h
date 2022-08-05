#ifndef MAPSUCTCHEONITEM_H
#define MAPSUCTCHEONITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsItem>
#include <QGeoCoordinate>
#include <QFont>
#include <QPen>
#include <QMutex>

class MapObjectItem;
class MapTableItem;

class GRAPHICSMAPLIB_EXPORT MapSuctcheonItem : public QObject, public QGraphicsItem
{
    /*line of Scutcheon.*/
    class MapScutcheonLine :public QGraphicsLineItem
    {
    public:
        explicit MapScutcheonLine(QGraphicsItem *parent = Q_NULLPTR)
            : QGraphicsLineItem(parent){}
        ~MapScutcheonLine(){}
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    };
    Q_OBJECT
public:
    /// 继承父对象将同步一切事件，但旋转会导致子对象一起旋转。
    MapSuctcheonItem(MapObjectItem * parent = nullptr);
    ~MapSuctcheonItem();
    /// 设置鼠标可拖拽
    void setMoveable(bool movable);
    /// 设置选中性
    void setCheckable(bool checkable);
    /// 设置选中状态(不会触发信号)
    void setChecked(bool checked);
    /// 切换选中状态(不会触发信号)
    void toggle();
    /// 是否选中
    bool isChecked() const;
    /// 设置边距和间距
    void setMargins(int left, int top, int right, int bottom);
    void setSpacing(int space);
    /// 设置连线图表的角度和长度  params: 这里长度为屏幕坐标,由于图表为ItemIgnoresTransformations 所以只需计算屏幕的终点位置
    void setOffset(qreal joinLineLength, int fixedAngle);
    void setOffset(QPointF endPos);
    /// 添加字段. params: 1.field  字段名. 2.bVolatile 标记字段值是否为易变性.
    /// 注：bVolatile=true表示字段值经常改变,此时会在paint中绘制. Volatile=false表示字段值比较固定,绘制更改时绘制一次,不在paint中实时绘制.
    void addField(const QString &field, bool bVolatile);
    void inrFiled(int pos, const QString &field, bool bVolatile);
    void delField(const QString &field);
    /// 设置字段名字体. params: 1.field 字段名.  2.font 字段名字体.   3.pen 字段名画笔
    void setFieldFont(const QString &field, const QFont &font);
    void setValueFont(const QString &field, const QFont &font);
    void setFieldPen(const QString &field, const QPen &pen);
    void setValuePen(const QString &field, const QPen &pen);
    /// 设置字段值. params: 1.field 字段名.   2.value 字段值.
    void setValue(const QString &field, const QString &value);
    /// 设置启用固定方向. 添加作为子项时,会跟随父项转动；此时若想固定方向，启用固定方向，并调用SetHeading将angle设置为父转动角度负值.
    /// params: 1.bFiexdDirect 标识是否启用固定方向. 2.fixedangle   固定角度值.
    void setFixedDirect(bool bFiexdDirect, qreal fixedangle = 0.0);
    /// 设置边框画笔.
    void setBorderPen(const QPen &borderPen);
    /// 设置背景画刷.
    void setBackBrush(const QBrush &brush);
    /// 设置连线画笔.
    void setJoinPen(const QPen &joinPen);
    /// 手动更新图表
    void updateTableSize();

    MapTableItem * getTabel() { return m_pTablet; }
protected:
    QPainterPath shape()  const override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
signals:
    void pressed();
    void released();
    void coordinateDragged(const QGeoCoordinate &coord);
    void doubleClicked();
private:
    void onCoordinateDragged(const QPointF& point);
private:
    QGraphicsRectItem       m_border;   /// 边框
    bool                    m_checkable = false;
    bool                    m_checked = false;
    bool					m_bFixedDirect; /// 固定方向.
    QPen					m_JoinPen;  /// 连线画笔.
    MapScutcheonLine		*m_pJoinLine; /// 连线对象.
    MapTableItem            *m_pTablet;  /// 标牌匾对象.
};


#endif // MAPSUCTCHEONITEM_H
