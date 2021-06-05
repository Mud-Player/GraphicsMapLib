#ifndef INTERACTIVEMAP_H
#define INTERACTIVEMAP_H

#include "graphicsmap.h"

class InteractiveMapOperator;
class MapEllipseItem;
class MapPolygonItem;
class MapRouteItem;
class MapObjectItem;

/*!
 * \brief 可交互地图
 * \details 该地图可实现鼠标缩放地图，配合Operator操作器实现地图对象的创建
 */
class GRAPHICSMAPLIB_EXPORT InteractiveMap : public GraphicsMap
{
    Q_OBJECT
public:
    InteractiveMap(QGraphicsScene *scene, QWidget *parent = nullptr);
    /// 设置事件交互操作器，传nullptr可以取消设置
    void setOperator(InteractiveMapOperator *op = nullptr);
    /// 清空所有地图元素
    void clear();
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

/*!
 * \brief 圆形创建操作器
 * \details 右键、双击完成创建
 */
class GRAPHICSMAPLIB_EXPORT MapEllipseOperator : public InteractiveMapOperator
{
    Q_OBJECT

public:
    MapEllipseOperator(QObject *parent = nullptr);

signals:
    void created(MapEllipseItem *item);

protected:
    virtual void ready() override;
    virtual void end() override;
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool mouseMoveEvent(QMouseEvent *event) override;

private:
    bool            m_ignoreEvent;
    QGeoCoordinate  m_first;
    MapEllipseItem *m_ellipse;
};

/*!
 * \brief 多边形创建操作器
 * \details 双击和右键可以结束编辑
 */
class GRAPHICSMAPLIB_EXPORT MapPolygonOperator : public InteractiveMapOperator
{
    Q_OBJECT

public:
    MapPolygonOperator(QObject *parent = nullptr);

signals:
    void created(MapPolygonItem *item);

protected:
    virtual void ready() override;
    virtual void end() override;
    virtual bool keyPressEvent(QKeyEvent *event) override;
    virtual bool mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool            m_finishRequested;
    QPoint          m_pressPos;
    MapPolygonItem *m_polygon;
};

/*!
 * \brief 图标对象创建操作器
 * \details 双击完成单图标对象的创建，多次点击实现图标初始初始位置与移动路线的规划
 */
class GRAPHICSMAPLIB_EXPORT MapObjectOperator : public InteractiveMapOperator
{
    Q_OBJECT

public:
    MapObjectOperator(QObject *parent = nullptr);

signals:
    void created(MapObjectItem *item);
    void created(MapRouteItem *item);

protected:
    virtual void ready() override;
    virtual void end() override;
    virtual bool keyPressEvent(QKeyEvent *event) override;
    virtual bool mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool           m_finishRequested;
    QPoint         m_pressPos;
    //
    MapObjectItem *m_object;
    MapRouteItem  *m_route;
};
#endif // INTERACTIVEMAP_H
