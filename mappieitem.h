#ifndef MAPPIEITEM_H
#define MAPPIEITEM_H

#include <GraphicsMapLib_global.h>
#include <QGraphicsEllipseItem>
#include <QGeoCoordinate>
#include <QSet>

/*!
 * \brief 扇形
 * \details 可用于SAR、红外等效果的扇形区域
 */
class GRAPHICSMAPLIB_EXPORT MapPieItem : public QGraphicsEllipseItem
{
public:
    MapPieItem();
    ~MapPieItem();
    /// 设置位置
    void setCoordinate(const QGeoCoordinate &coord);
    /// 设置长度
    void setRadius(const qreal &meter);
    /// 设置朝向
    void setAzimuth(const qreal &degree);
    /// 设置张角
    void setAngle(const qreal &degree);

public:
    /// 获取所有的实例
    static const QSet<MapPieItem*> &items();

private:
    void updatePie();

private:
    static QSet<MapPieItem*> m_items;         ///< 所有实例

private:
    QGeoCoordinate m_coord;     ///< 位置
    qreal          m_radius;    ///< 长度(半径)
    qreal          m_azimuth;   ///< 朝向，以正北右偏为正
    qreal          m_span;      ///< 张角 度
};

#endif // MAPPIEITEM_H
