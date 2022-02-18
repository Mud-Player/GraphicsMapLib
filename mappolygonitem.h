#ifndef MAPPOLYGONITEM_H
#define MAPPOLYGONITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsItemGroup>
#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QGeoCoordinate>

/*!
 * \brief 多边形
 * \note 暂缺少图形拖动的实现
 */
class GRAPHICSMAPLIB_EXPORT MapPolygonItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    MapPolygonItem();
    ~MapPolygonItem();
    /// 控制可编辑性
    void setEditable(bool editable);
    bool isEditable() const;
    void toggleEditable();
    /// 添加经纬点
    void append(const QGeoCoordinate &coord);
    /// 修改经纬点
    void replace(const int &index, const QGeoCoordinate &coord);
    /// 删除经纬点
    void remove(int index);
    void removeEnd();
    /// 设置多边形顶点
    void setPoints(const QVector<QGeoCoordinate> &coords);
    /// 获取多边形顶点
    const QVector<QGeoCoordinate> &points() const;
    int count();
    /// 获取某个点的位置
    const QGeoCoordinate &at(int i) const;

public:
    /// 获取所有的实例
    static const QSet<MapPolygonItem*> &items();

signals:
    void added(const int index, const QGeoCoordinate &coord);
    void removed(const int index, const QGeoCoordinate &coord);
    void updated(const int &index, const QGeoCoordinate &coord);
    void changed();
    void doubleClicked();
    void editableChanged(bool editable);

protected:
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;
    /// 被添加到场景后，为控制点添加事件过滤器
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void updatePolygon();   ///< 通过场景坐标更新图形
    void updateEditable();

private:
    static QSet<MapPolygonItem*> m_items;         ///< 所有实例

private:
    bool    m_editable;   ///< 鼠标是否可交互编辑
    bool    m_sceneAdded; ///< 是否已被添加到场景
    //
    QVector<QGeoCoordinate>      m_coords;     ///< 经纬点列表
    QVector<QPointF>             m_points;     ///< 场景坐标点列表
    //
    QVector<QGraphicsEllipseItem*> m_ctrlPoints; ///< 控制点(圆点)
};

#endif // MAPPOLYGONITEM_H
