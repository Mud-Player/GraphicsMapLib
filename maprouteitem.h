#ifndef MAPROUTEITEM_H
#define MAPROUTEITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsPathItem>
#include <QGeoCoordinate>
#include <QPen>

class MapObjectItem;

/*!
 * \brief 航路
 */
class GRAPHICSMAPLIB_EXPORT MapRouteItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    /// 航点定义
    struct Point {
        QGeoCoordinate coord;   ///< 经纬高
        float          speed;   ///< 速度 km/h
        inline bool operator==(const Point &rhs) const {
            return (this->coord == rhs.coord && this->speed == rhs.speed);
        };
    };
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
    /// 是否自动航点编号，默认打开从0编号
    void setAutoNumber(bool on);
    /// 添加航点
    MapObjectItem *append(const Point &point);
    /// 插入航点
    MapObjectItem *insert(const int &index, const Point &point);
    /// 替换航点
    MapObjectItem *replace(const int &index, const Point &point);
    /// 删除航点
    void remove(const int &index);
    /// 设置航点
    const QVector<MapObjectItem*> &setPoints(const QVector<Point> &points);
    /// 获取航点列表
    const QVector<Point> &points() const;

public:
    /// 获取所有的实例
    static const QSet<MapRouteItem*> &items();

signals:
    void added(const int &index, const Point &point);
    void removed(const int &index, Point &point);
    void updated(const int &index, const Point &point);
    void changed();

private:
    static QSet<MapRouteItem*> m_items;         ///< 所有实例

private:
    void updatePolylineAndText(int beginIndex , int endIndex);    ///< 更新航线和航点文字，beginIndex可以用于优化从指定位置开始更新
    void updateByPointMove();
    void checkByPointPress();
    MapObjectItem *createPoint(const QGeoCoordinate &coord);

private:
    QPen m_normalPen;    ///< 非编辑状态画笔
    QPen m_editablePen;  ///< 编辑状态画笔
    bool m_editable;     ///< 鼠标是否可交互编辑
    bool m_checkable;    ///< 航点可选中性
    bool m_autoNumber;   ///< 航点自动编号
    int  m_checkedIndex; ///< 当前选中航点
    //
    QVector<Point>            m_routePoints;    ///< 航点数据
    QVector<MapObjectItem*>   m_ctrlItems;      ///< 航点元素
};
Q_DECLARE_METATYPE(MapRouteItem::Point)

#endif // MAPROUTEITEM_H
