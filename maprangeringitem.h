#ifndef MAPRANGERINGITEM_H
#define MAPRANGERINGITEM_H

#include "GraphicsMapLib_global.h"
#include <QObject>
#include <QGeoCoordinate>
#include <QGraphicsItem>
#include <QPen>
#include <QFont>

class MapObjectItem;

/*!
 * \brief 距离环
 * \details 显示三个地理等距椭圆环，如果需要实现屏幕恒等大小的效果，需要外部手动调用setRadius接口来实现
 * \warning 该元素对渲染效率影响较大，会占用过多CPU，暂未找到好的解决方案
 */
class GRAPHICSMAPLIB_EXPORT MapRangeRingItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit MapRangeRingItem();
    /// 设置位置
    void setCoordinate(const QGeoCoordinate &coord);
    /// 设置半径
    void setRadius(const float &km);
    /// 依附到地图对象，将会自动更新位置
    void attach(MapObjectItem *obj);
    /// 取消依附地图对象，后续手动更新位置
    void detach();
    /// 设置画笔
    void setPen(const QPen &pen);
    /// 设置字体
    void setFont(const QFont &font);
    /// 获取画笔
    QPen pen() const;
    /// 获取字体
    QFont font() const;

public:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    void updateBoundingRect();
    void drawEllipse(QPainter *painter, const float &radius);

private:
    QPen   m_pen;
    QFont  m_font;
    //
    QGeoCoordinate m_coord;
    float          m_radius;
    //
    QRectF m_boundRect;
    //
    MapObjectItem *m_attachObj;
};

#endif // MAPRANGERINGITEM_H
