#ifndef MAPPOLYGONITEM_H
#define MAPPOLYGONITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QGeoCoordinate>

/*!
 * \brief 多边形
 */
class GRAPHICSMAPLIB_EXPORT MapPolygonItem : public QGraphicsPolygonItem
{
public:
    MapPolygonItem();
    /// 控制可编辑性
    void setEditable(const bool &editable);
    /// 添加经纬点
    void addCoordinate(const QGeoCoordinate &coord);
    /// 修改经纬点
    void replaceCoordinate(const int &index, const QGeoCoordinate &coord);
    /// 删除经纬点
    void removeCoordinate(const int &index);
    /// 设置多边形顶点
    void setCoordinates(const QList<QGeoCoordinate> &coords);
    /// 获取多边形顶点
    const QList<QGeoCoordinate> &coordinates() const;

protected:
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;


private:
    void updatePolygon();
    void updateEditable();
private:
    bool    m_editable;   ///< 鼠标是否可交互编辑
    QPointF m_center;     ///< 包围框中心
    //
    QList<QGeoCoordinate>        m_coords;     ///< 经纬点列表
    QList<QGraphicsEllipseItem*> m_ctrlPoints; ///< 控制点(圆点)
};

#endif // MAPPOLYGONITEM_H
