#ifndef MAPROUTEITEM_H
#define MAPROUTEITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsPathItem>
#include <QGeoCoordinate>
#include <QPen>

class MapObjectItem;

/*!
 * \brief 航路
 * \note 航路类将负责航点的生命周期
 */
class GRAPHICSMAPLIB_EXPORT MapRouteItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    using QGraphicsPathItem::setPen;
    using QGraphicsPathItem::pen;

    MapRouteItem();
    ~MapRouteItem();
    /// 控制可编辑性
    void setEditable(const bool &editable);
    /// 设置航点可选中
    void setCheckable(const bool &checkable);
    /// 设置航点被选中
    void setChecked(int index);
    /// 当前选中的航点，-1为空
    int checked() const;
    /// 设置编辑状态和非编辑状态下两种画笔
    void setPen(bool editable, const QPen &pen);
    /// 获取画笔
    QPen pen(bool editable) const;
    /// 添加航点
    MapObjectItem *append(MapObjectItem *point);
    MapObjectItem *append(const QGeoCoordinate &coord);
    /// 插入航点
    MapObjectItem *insert(const int &index, MapObjectItem *point);
    MapObjectItem *insert(const int &index, const QGeoCoordinate &coord);
    /// 替换航点
    MapObjectItem *replace(const int &index, MapObjectItem *point);
    MapObjectItem *replace(const int &index, const QGeoCoordinate &coord);
    /// 删除航点
    void remove(const int &index);
    /// 设置航点
    const QVector<MapObjectItem*> &setPoints(const QVector<MapObjectItem*> &points);
    /// 获取航点列表
    const QVector<MapObjectItem*> &points() const;

public:
    /// 获取所有的实例
    static const QSet<MapRouteItem*> &items();

signals:
    void added(const int &index, const MapObjectItem *point);
    void removed(const int &index);
    void updated(const int &index, const MapObjectItem *point);
    void changed();

private:
    static QSet<MapRouteItem*> m_items;         ///< 所有实例

private:
    void updatePolyline();
    void updateByPointMove();
    void checkByPointPress();
    void bindPoint(MapObjectItem *point);

private:
    QPen m_normalPen;    ///< 非编辑状态画笔
    QPen m_editablePen;  ///< 编辑状态画笔
    bool m_editable;     ///< 鼠标是否可交互编辑
    bool m_checkable;    ///< 航点可选中性
    int  m_checkedIndex; ///< 当前选中航点
    //
    QVector<MapObjectItem*>   m_points;      ///< 航点元素
};

#endif // MAPROUTEITEM_H
