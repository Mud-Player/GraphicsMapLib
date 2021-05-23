#ifndef INTERACTIVEMAP_H
#define INTERACTIVEMAP_H

#include "graphicsmap.h"

class InteractiveMapOperator;
class MapEllipseItem;

/*!
 * \brief 可交互地图
 * \details 该地图可实现鼠标缩放地图，配合Operator操作器实现地图对象的创建
 */
class GRAPHICSMAPLIB_EXPORT InteractiveMap : public GraphicsMap
{
    Q_OBJECT
public:
    InteractiveMap(QGraphicsScene *scene);
    /// 设置事件交互操作器，传nullptr可以取消设置
    void setOperator(InteractiveMapOperator *op = nullptr);

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
    InteractiveMapOperator *m_operator;
};

/*!
 * \brief 可交互操作器
 * \details InteractiveMap将会调用该类的事件接口，以提供固定的地图处理功能，比如创建一个圆形
 * \note 在多个事件处理函数中，如果返回true，表示事件已被处理不希望再被传递，反之false表示希望该事件继续被传递处理
 */
class GRAPHICSMAPLIB_EXPORT InteractiveMapOperator
{
    friend InteractiveMap;
public:
    inline QGraphicsScene *scene() const {return m_scene;};
    inline GraphicsMap *map() const {return m_map;};

private:
    inline void setScene(QGraphicsScene *scene) {m_scene = scene;};
    void setMap(GraphicsMap *map) {m_map = map;};
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
    GraphicsMap    *m_map;
};

/*!
 * \brief 圆形创建操作器
 */
class GRAPHICSMAPLIB_EXPORT MapEllipseOperator : public InteractiveMapOperator
{
public:
    MapEllipseOperator();
protected:
    virtual void ready() override;
    virtual void end() override;
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool mouseMoveEvent(QMouseEvent *event) override;

private:
    bool            m_ignoreEvent;
    MapEllipseItem *m_ellipse;
    QGeoCoordinate m_first;
};

#endif // INTERACTIVEMAP_H
