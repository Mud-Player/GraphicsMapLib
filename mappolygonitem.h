#ifndef MAPPOLYGONITEM_H
#define MAPPOLYGONITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsItemGroup>
#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QGeoCoordinate>

/*!
 * \brief 多边形
 */
class GRAPHICSMAPLIB_EXPORT MapPolygonItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
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
    void setCoordinates(const QVector<QGeoCoordinate> &coords);
    /// 获取多边形顶点
    const QVector<QGeoCoordinate> &coordinates() const;

signals:
    void added(int index, const QGeoCoordinate &coord);
    void removed(int index, const QGeoCoordinate &coord);
    void updated(const int &index, const QGeoCoordinate &coord);
    void changed();

protected:
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;

private:
    void updatePolygon();   ///< 通过场景坐标更新图形
    void updateEditable();
private:
    bool    m_editable;   ///< 鼠标是否可交互编辑
    //
    QVector<QGeoCoordinate>      m_coords;     ///< 经纬点列表
    QVector<QPointF>             m_points;     ///< 场景坐标点列表
    //
    QVector<QGraphicsEllipseItem*> m_ctrlPoints; ///< 控制点(圆点)
};

#endif // MAPPOLYGONITEM_H
