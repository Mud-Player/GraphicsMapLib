#ifndef MAPTRAILITEM_H
#define MAPTRAILITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsPathItem>
#include <QGeoCoordinate>

/*!
 * \brief 轨迹
 */
class GRAPHICSMAPLIB_EXPORT MapTrailItem : public QGraphicsPathItem
{
public:
    MapTrailItem();
    /// 添加经纬点轨迹（该函数会自动优化该点是否添加到轨迹点）
    void addCoordinate(const QGeoCoordinate &coord);
    /// 清除轨迹
    void clear();

private:
    QGeoCoordinate m_coord;    ///< 轨迹点0

};

#endif // MAPTRAILITEM_H
