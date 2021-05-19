#ifndef MAPELLIPSEITEM_H
#define MAPELLIPSEITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsEllipseItem>
#include <QGeoCoordinate>

/*!
 * \brief 椭圆形/正圆
 * \note 拖动控制点时，按住Shift固定中心进行缩放，否则以对角包围矩形任意大小缩放
 */
class GRAPHICSMAPLIB_EXPORT MapEllipseItem : public QGraphicsEllipseItem
{
public:
    MapEllipseItem();
    /// 控制可编辑性
    void setEditable(const bool &editable);
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

private:
    void updateEllipse();
    void updateEditable();
private:
    bool           m_editable;   ///< 鼠标是否可交互编辑
    QGeoCoordinate m_center;     ///< 中心
    QSizeF         m_size;       ///< 宽高
    //
    QGraphicsEllipseItem m_topLeftCtrl;
    QGraphicsEllipseItem m_bottomRightCtrl;
};

#endif // MAPELLIPSEITEM_H
