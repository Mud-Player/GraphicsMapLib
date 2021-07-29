#ifndef MAPPIEITEM_H
#define MAPPIEITEM_H

#include <GraphicsMapLib_global.h>
#include <QGraphicsEllipseItem>
#include <QGeoCoordinate>
#include <QSet>

class MapObjectItem;

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
    qreal          m_radius;    ///< 长度 米
    qreal          m_azimuth;   ///< 朝向，以正北右偏为正
    qreal          m_span;      ///< 张角 度
};

/*!
 * \brief 三角形梯形混合类扇形
 * \details 可用于SAR、红外等效果的威力区
 * \note 更改三角形和梯形外观，请通过getTriangle和getTrapezoid获取实例，然后更改画笔和画刷
 */
class GRAPHICSMAPLIB_EXPORT MapTriTrapItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    MapTriTrapItem();
    ~MapTriTrapItem();
    /// 设置位置
    void setCoordinate(const QGeoCoordinate &coord);
    /// 设置近距长度
    void setNear(const qreal &meter);
    /// 设置远距长度
    void setFar(const qreal &meter);
    /// 设置朝向
    void setAzimuth(const qreal &degree);
    /// 设置张角
    void setAngle(const qreal &degree);
    /// 获取三角形
    QGraphicsPolygonItem *getTriangle();
    /// 获取梯形
    QGraphicsPolygonItem *getTrapezoid();
    /// 依附到地图对象，将会自动更新位置
    void attach(MapObjectItem *obj);
    /// 取消依附地图对象，后续手动更新位置
    void detach();

public:
    /// 获取所有的实例
    static const QSet<MapTriTrapItem*> &items();

private:
    void updateTrapezoid();
    void on_attachRotationChanged(const qreal &degree);

private:
    static QSet<MapTriTrapItem*> m_items;         ///< 所有实例

private:
    QGeoCoordinate m_coord;     ///< 位置
    qreal          m_near;      ///< 近距 米
    qreal          m_far;       ///< 远距 米
    qreal          m_azimuth;   ///< 朝向，以正北右偏为正
    qreal          m_span;      ///< 张角 度
    //
    QGraphicsPolygonItem m_triangle;
    QGraphicsPolygonItem m_trapezoid;
    //
    MapObjectItem *m_attachObj;
    qreal          m_attachAzimuth;
};
#endif // MAPPIEITEM_H
