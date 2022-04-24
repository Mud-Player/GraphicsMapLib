#ifndef MAPTRAILITEM_H
#define MAPTRAILITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsPathItem>
#include <QGeoCoordinate>

class MapObjectItem;

/*!
 * \brief 轨迹
 * \warning 该元素对渲染效率影响较大，会占用过多CPU，暂未找到好的解决方案
 */
class GRAPHICSMAPLIB_EXPORT MapTrailItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    MapTrailItem();
    ~MapTrailItem();
    /// 添加经纬点轨迹（该函数会自动优化该点是否添加到轨迹点）
    void addCoordinate(const QGeoCoordinate &coord);
    /// 清除轨迹
    void clear();
    /// 依附到地图对象，清除已存在的航迹，将会自动更新位置
    void attach(MapObjectItem *obj);
    /// 取消依附地图对象，后续手动更新位置，如需清除航迹，请手动清除
    void detach();

public:
    /// 获取所有的实例
    static const QSet<MapTrailItem*> &items();

private:
    static QSet<MapTrailItem*> m_items;         ///< 所有实例
private:
    QGeoCoordinate m_coord;    ///< 轨迹点0
    //
    MapObjectItem *m_attachObj;
};

#endif // MAPTRAILITEM_H
