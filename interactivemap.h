#ifndef INTERACTIVEMAP_H
#define INTERACTIVEMAP_H

#include "graphicsmap.h"

class InteractiveMapOperator;
class MapEllipseItem;
class MapPolygonItem;
class MapRouteItem;
class MapObjectItem;
class MapRangeRingItem;
class MapTrailItem;

/*!
 * \brief 可交互地图
 * \details 1.提供众多对地图元素添加、获取的接口
 * 2.该地图可实现鼠标缩放地图，配合Operator操作器实现地图对象的创建（预设了几个简单的操作器，复杂操作请自己继承实现）
 * \note 通过该类管理的对象，不要手动从场景移除，请调用该类的删除接口
 */
class GRAPHICSMAPLIB_EXPORT InteractiveMap : public GraphicsMap
{
    Q_OBJECT
public:
    InteractiveMap(QGraphicsScene *scene, QWidget *parent = nullptr);

    /// 创建圆形，之前已有相同ID的将会被覆盖
    MapEllipseItem *addMapEllipse(int id);
    /// 将自创建的圆形添加到该类通过ID管理，相同ID的将会被覆盖
    void addMapEllipse(int id, MapEllipseItem *item);
    /// 获取圆形
    MapEllipseItem *getMapEllipse(int id);
    /// 删除圆形
    bool removeMapEllipse(int id);
    /// 清空该类管理的所有圆形
    void clearMapEllipse();

    /// 创建图标对象，之前已有相同ID的将会被覆盖
    MapObjectItem *addMapObject(int id);
    /// 将自创建的图标对象添加到该类通过ID管理，相同ID的将会被覆盖
    void addMapObject(int id, MapObjectItem *item);
    /// 获取图标对象
    MapObjectItem *getMapObject(int id);
    /// 删除图标对象
    bool removeMapObject(int id);
    /// 清空该类管理的所有图标对象
    void clearMapObject();

    /// 创建多边形，之前已有相同ID的将会被覆盖
    MapPolygonItem *addMapPolygon(int id);
    /// 将自创建的多边形添加到该类通过ID管理，相同ID的将会被覆盖
    void addMapPolygon(int id, MapPolygonItem *item);
    /// 获取多边形
    MapPolygonItem *getMapPolygon(int id);
    /// 删除多边形
    bool removeMapPolygon(int id);
    /// 清空该类管理的所有多边形
    void clearMapPolygon();

    /// 创建距离环，之前已有相同ID的将会被覆盖
    MapRangeRingItem *addMapRangeRing(int id);
    /// 将自创建的距离环添加到该类通过ID管理，相同ID的将会被覆盖
    void addMapRangeRing(int id, MapRangeRingItem *item);
    /// 获取距离环
    MapRangeRingItem *getMapRangeRing(int id);
    /// 删除距离环
    bool removeMapRangeRing(int id);
    /// 清空该类管理的所有距离环
    void clearMapRangeRing();

    /// 创建航路，之前已有相同ID的将会被覆盖
    MapRouteItem *addMapRoute(int id);
    /// 将自创建的航路添加到该类通过ID管理，相同ID的将会被覆盖
    void addMapRoute(int id, MapRouteItem *item);
    /// 获取航路
    MapRouteItem *getMapRoute(int id);
    /// 删除多边形
    bool removeMapRoute(int id);
    /// 清空该类管理的所有航路
    void clearMapRoute();

    /// 创建轨迹，之前已有相同ID的将会被覆盖
    MapTrailItem *addMapTrail(int id);
    /// 将自创建的航路添加到该类通过ID管理，相同ID的将会被覆盖
    void addMapTrail(int id, MapTrailItem *item);
    /// 获取距离环
    MapTrailItem *getMapTrail(int id);
    /// 删除多边形
    bool removeMapTrail(int id);
    /// 清空该类管理的所有轨迹
    void clearMapTrail();

    /// 清空该类管理的所有地图元素（若需要删除Operator创建的元素，请将元素通过addXX(int ID)接口添加到管理)
    void clear();

    /// 设置事件交互操作器，传nullptr可以取消设置
    void setOperator(InteractiveMapOperator *op = nullptr);
    /// 保持对象居中，传空值可以取消设置
    void setCenter(const MapObjectItem *obj);
    /// 设置鼠标是否可以交互缩放
    void setScaleable(bool on);

protected:
    virtual void wheelEvent(QWheelEvent *e) override;
    //  将要传递给操作器的事件
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QHash<int, MapEllipseItem*>   m_ellipseHash;
    QHash<int, MapObjectItem*>    m_objectHash;
    QHash<int, MapPolygonItem*>   m_polygonHash;
    QHash<int, MapRangeRingItem*> m_rangeRingHash;
    QHash<int, MapRouteItem*>     m_routeHash;
    QHash<int, MapTrailItem*>     m_trailHash;
    //
    InteractiveMapOperator *m_operator;     ///< 操作器
    const MapObjectItem    *m_centerObj;    ///< 居中对象
    QGraphicsView::DragMode m_dragMode;     ///< 拖拽模式(用于取消居中之后回到之前的模式)
    QGraphicsView::ViewportAnchor m_anchor; ///< 鼠标锚点(用于取消居中之后回到之前的模式)
    //
    bool  m_scaleable;  ///< 是否可以鼠标缩放
};

/*!
 * \brief 可交互操作器
 * \details InteractiveMap将会调用该类的事件接口，以提供固定的地图处理功能，比如创建一个圆形
 * \note 在多个事件处理函数中，如果返回true，表示事件已被处理不希望再被传递，反之false表示希望该事件继续被传递处理
 */
class GRAPHICSMAPLIB_EXPORT InteractiveMapOperator : public QObject
{
    Q_OBJECT
    friend InteractiveMap;
public:
    InteractiveMapOperator(QObject *parent = nullptr);
    inline QGraphicsScene *scene() const {return m_scene;};
    inline GraphicsMap *map() const {return m_map;};

private:
    inline void setScene(QGraphicsScene *scene) {m_scene = scene;};
    inline void setMap(InteractiveMap *map) {m_map = map;};

protected:
    virtual void ready(){}; /// 重新被设置为操作器的时候将会被调用
    virtual void end(){};   /// 操作器被取消的时候将会被调用
    virtual bool keyPressEvent(QKeyEvent *) {return false;};
    virtual bool keyReleaseEvent(QKeyEvent *) {return false;};
    virtual bool mouseDoubleClickEvent(QMouseEvent *) {return false;};
    virtual bool mouseMoveEvent(QMouseEvent *) {return false;};
    virtual bool mousePressEvent(QMouseEvent *) {return false;};
    virtual bool mouseReleaseEvent(QMouseEvent *) {return false;};

protected:
    QGraphicsScene *m_scene;
    InteractiveMap *m_map;
};

#endif // INTERACTIVEMAP_H
