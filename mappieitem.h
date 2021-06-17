#ifndef MAPPIEITEM_H
#define MAPPIEITEM_H

#include <GraphicsMapLib_global.h>
#include <QGraphicsEllipseItem>
#include <QGeoCoordinate>

/*!
 * \brief 扇形
 * \details 可用于SAR、红外等效果的扇形区域
 */
class GRAPHICSMAPLIB_EXPORT MapPieItem : public QGraphicsEllipseItem
{
public:
    MapPieItem();
    /// 设置位置
    void setCoordinate(const QGeoCoordinate &coord);
    /// 设置距离
    void setDistance(const qreal &meter);
    /// 设置半径
    void setRadius(const qreal &meter);
private:
    QGeoCoordinate m_coord;
    qreal          m_distance;
    qreal          m_radius;
};

#endif // MAPPIEITEM_H
