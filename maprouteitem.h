#ifndef MAPROUTEITEM_H
#define MAPROUTEITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsPathItem>
#include <QGeoCoordinate>

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
    public:
        bool operator==(const Point &rhs);
    };
public:
    MapRouteItem();
    /// 添加航点
    void append(const Point &point);
    /// 插入航点
    void insert(const int &index, const Point &point);
    /// 替换航点
    void replace(const int &index, const Point &point);
    /// 删除航点
    void remove(const int &index);
    /// 设置航点
    void setPoints(const QVector<Point> &points);
    /// 获取航点列表
    const QVector<Point> &points() const;

signals:
    void added(const int &index, const Point &point);
    void removed(const int &index, Point &point);
    void updated(const int &index, const Point &point);
    void changed();

private:
    void updateRoute();

private:
    QVector<Point>   m_points;     ///< 航点列表
};
Q_DECLARE_METATYPE(MapRouteItem::Point)

#endif // MAPROUTEITEM_H
