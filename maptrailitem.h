#ifndef MAPTRAILITEM_H
#define MAPTRAILITEM_H

#include <QGraphicsPathItem>
#include <QGeoCoordinate>

/*!
 * \brief 轨迹
 */
class maptrailitem : public QGraphicsPathItem
{
public:
    maptrailitem();
    /// 添加经纬点轨迹（该函数会自动优化该点是否添加到轨迹点）
    void addCoordinate(const QGeoCoordinate &coord);

private:
    QGeoCoordinate m_coord0;    ///< 轨迹点0
    QGeoCoordinate m_coord1;    ///< 轨迹顶1

};

#endif // MAPTRAILITEM_H
