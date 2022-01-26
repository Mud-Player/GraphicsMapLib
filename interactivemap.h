#ifndef INTERACTIVEMAP_H
#define INTERACTIVEMAP_H

#include "graphicsmap.h"

class InteractiveMapOperator;
class MapObjectItem;

/*!
 * \brief 可交互地图
 * \details 1.提供众多对地图元素添加、获取的模板接口
 * 2.该地图可实现鼠标缩放地图，配合Operator操作器实现地图对象的创建（预设了几个简单的操作器，复杂操作请自己继承实现）
 * \note 通过该类管理的对象，不要手动从场景移除，请调用该类的删除接口
 */
class GRAPHICSMAPLIB_EXPORT InteractiveMap : public GraphicsMap
{
    Q_OBJECT
public:
    InteractiveMap(QWidget *parent = nullptr);

    /// 创建地图元素
    template<class T>
    T *addMapItem();
    /// 删除地图元素
    template<class T>
    void removeMapItem(T* item);
    /// 清空该类管理的所有圆形
    template<class T>
    void clearMapItem();

    /// 设置事件交互操作器
    void setOperator(InteractiveMapOperator *op = nullptr);
    void unsetOperator();
    /// 保持对象居中，传空值可以取消设置
    void setCenter(const MapObjectItem *obj);
    /// 设置鼠标是否可以交互缩放
    void setZoomable(bool on);

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

template<class T>
T *InteractiveMap::addMapItem()
{
    auto item = new T();
    scene()->addItem(item);
    return item;
}

template<class T>
void InteractiveMap::removeMapItem(T *item)
{
    scene()->removeItem(item);
    delete item;
}

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
    /// 操作模式
    enum OperatorMode {
        CreateOnly,   ///< 创建模式
        EditOnly,     ///< 编辑模式
        CreateEdit    ///< 创建+编辑
    };

    InteractiveMapOperator(QObject *parent = nullptr);

    /// 设置操作模式
    inline void setMode(OperatorMode mode) { m_mode = mode;}
    inline OperatorMode mode() const { return m_mode;}

    /// 设置是否为瞬态操作器，当其completed之后将会自动被地图取消
    inline void setTransient(bool transient) { m_transient = transient;};
    inline bool isTransient() const {return m_transient;}

    inline QGraphicsScene *scene() const {return m_scene;};
    inline GraphicsMap *map() const {return m_map;};
    inline bool isIgnoreKeyEventLoop() const {return m_ignoreKeyEventLoop;}
    inline bool isIgnoreMouseEventLoop() const {return m_ignoreMouseEventLoop;}

signals:
    /// 编辑完成信号
    void completed();

private:
    inline void setScene(QGraphicsScene *scene) {m_scene = scene;};
    inline void setMap(InteractiveMap *map) {m_map = map;};

protected:
    /// 忽略一次按键事件循环，从press到release的事件，通常在mousePressEvent调用
    inline void ignoreKeyEventLoop() {m_ignoreKeyEventLoop = true;};
    /// 忽略一次鼠标事件循环，从press到release的事件，通常在keyPressEvent调用
    void ignoreMouseEventLoop() {m_ignoreMouseEventLoop = true;};


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

private:
    bool m_ignoreKeyEventLoop = false;
    bool m_ignoreMouseEventLoop = false;
    OperatorMode m_mode = CreateEdit;  ///< 操作模式
    bool m_transient = false;       ///< 操作器瞬态
};

#endif // INTERACTIVEMAP_H
