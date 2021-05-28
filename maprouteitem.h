﻿#ifndef MAPROUTEITEM_H
#define MAPROUTEITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsPathItem>
#include <QGeoCoordinate>

class MapRoutePoint;

/*!
 * \brief 航路
 */
class GRAPHICSMAPLIB_EXPORT MapRouteItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    /// 航点定义
    struct Point {
        QGeoCoordinate coord;   ///< 经纬高
        float          speed;   ///< 速度 km/h
        inline bool operator==(const Point &rhs) const {
            return (this->coord == rhs.coord && this->speed == rhs.speed);
        };
    };
public:
    MapRouteItem();
    /// 是否自动航点编号，默认打开从0编号
    void setAutoNumber(bool on);
    /// 添加航点
    MapRoutePoint *append(const Point &point);
    /// 插入航点
    MapRoutePoint *insert(const int &index, const Point &point);
    /// 替换航点
    MapRoutePoint *replace(const int &index, const Point &point);
    /// 删除航点
    void remove(const int &index);
    /// 设置航点
    const QVector<MapRoutePoint*> &setPoints(const QVector<Point> &points);
    /// 获取航点列表
    const QVector<MapRoutePoint*> &points() const;

signals:
    void added(const int &index, const Point &point);
    void removed(const int &index, Point &point);
    void updated(const int &index, const Point &point);
    void changed();

private:
    void updateRoute();
    MapRoutePoint *createPoint();

private:
    QVector<Point>            m_routePoints;    ///< 航点数据
    QVector<MapRoutePoint*>   m_ctrlItems;      ///< 航点元素
};
Q_DECLARE_METATYPE(MapRouteItem::Point)

/*!
 * \brief 航路点
 */
class MapRoutePoint : public QGraphicsEllipseItem {
public:
    MapRoutePoint();
    /// 设置文字
    void setText(const QString &text);
    /// 开启动画，文字和方向切换显示
    void setAnimation(bool on);

private:
    QGraphicsSimpleTextItem m_text;
};

#endif // MAPROUTEITEM_H