#ifndef MAPRECTITEM_H
#define MAPRECTITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsRectItem>
#include <QGeoCoordinate>

/*!
 * \brief 矩形
 * \note 暂缺少图形拖动的实现
 */
class GRAPHICSMAPLIB_EXPORT MapRectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    MapRectItem();
    ~MapRectItem();
    /// 控制可编辑性
    void setEditable(const bool &editable);
    bool isEditable() const;
    void toggleEditable();
    /// 设置中心
    void setCenter(const QGeoCoordinate &center);
    /// 设置尺寸 米
    void setSize(const QSizeF &size);
    /// 设置包围矩形两个对角顶点来自动生成圆形
    void setRect(const QGeoCoordinate &first, const QGeoCoordinate &second);
    /// 获取中心
    const QGeoCoordinate &center() const;
    /// 获取尺寸
    const QSizeF &size() const;
    /// 获取四个点的坐标,顺时针方向，左上角为第一个点
    QVector<QGeoCoordinate> points() const;

public:
    /// 获取所有的实例
    static const QSet<MapRectItem*> &items();

signals:
    void centerChanged(const QGeoCoordinate &center);
    void sizeChanged(const QSizeF &center);
    void doubleClicked();
    void editableChanged(bool editable);

protected:
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;
    /// 被添加到场景后，为控制点添加事件过滤器
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void updateRect();
    void updateEditable();

private:
    static QSet<MapRectItem*> m_items;         ///< 所有实例

private:
    bool           m_editable;   ///< 鼠标是否可交互编辑
    //
    QGeoCoordinate m_center;     ///< 中心
    QSizeF         m_size;       ///< 宽高
    //
    QGeoCoordinate m_topLeftCoord;        ///< 左上经纬度
    QGeoCoordinate m_bottomRightCoord;    ///< 右下经纬度
    //
    QGraphicsRectItem    m_rectCtrl;       ///< 矩形
    QGraphicsEllipseItem m_firstCtrl;      ///< 对角控制点1
    QGraphicsEllipseItem m_secondCtrl;     ///< 对角控制点2
};

#endif // MAPRECTITEM_H
