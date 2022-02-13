#ifndef INTERACTIVEMAP_H
#define INTERACTIVEMAP_H

#include "graphicsmap.h"
#include <QStack>

class MapOperator;
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

    /// 设置事件交互操作器(不可重复)
    bool pushOperator(MapOperator *op);
    bool popOperator();
    MapOperator *topOperator() const;
    void clearOperator();
    /// 保持对象居中，传空值可以取消设置
    void setCenter(const MapObjectItem *obj);
    /// 设置鼠标是否可以交互缩放
    void setZoomable(bool on);

protected:
    virtual void wheelEvent(QWheelEvent *e) override;
    //  将要传递给操作器的事件
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void onOperatorModeChanged();

private:
    QStack<MapOperator*> m_operators;     ///< 操作器栈
    const MapObjectItem    *m_centerObj;             ///< 居中对象
    QGraphicsView::DragMode m_dragMode;              ///< 拖拽模式(用于取消居中之后回到之前的模式)
    QGraphicsView::ViewportAnchor m_anchor;          ///< 鼠标锚点(用于取消居中之后回到之前的模式)
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
 * 另外，一个鼠标事件周期包含两种：press release 和 doubleClick release
 * \todo 为了统计操作器的交互习惯，建议左键双击完成编辑、右键赋予具体的编辑功能
 * \warning 重写子类的ready和end时，必须调用基类函数
 */
class GRAPHICSMAPLIB_EXPORT MapOperator : public QObject
{
    Q_OBJECT
    friend InteractiveMap;

public:
    /// 操作模式
    enum OperatorMode {
        EditOnly,     ///< 编辑模式，当其completed之后将会自动被取消(自动瞬态)
        CreateOnly,   ///< 创建模式
        CreateEdit    ///< 创建+编辑
    };

    MapOperator(QObject *parent = nullptr);

    /// 设置操作模式
    void setMode(OperatorMode mode);
    inline OperatorMode mode() const { return m_mode;}

    inline QGraphicsScene *scene() const {return m_scene;};
    inline GraphicsMap *map() const {return m_map;};

signals:
    /// 编辑完成信号
    void completed();
    void modeChanged(MapOperator::OperatorMode mode);

protected:
    /// 是否追踪鼠标移动事件(如果启用，moveEvent将不会首受到skipOnceMouseEvent的影响)
    inline void setMouseTracking(bool enable) {m_mouseMoveTrackingEnable = enable;}
    /// 忽略一次按键事件循环，从press到release的事件，通常在mousePressEvent调用
    inline void skipOnceKeyEvent() {m_skipOnceKeyEvent = true;}
    /// 忽略一次鼠标事件循环，从press到release的事件，通常在keyPressEvent调用
    inline void skipOnceMouseEvent() {m_skipOnceMouseEvent = true;}


protected:
    virtual void ready(); /// 重新被设置为操作器的时候将会被调用
    virtual void end();   /// 操作器被取消的时候将会被调用
    virtual bool keyPressEvent(QKeyEvent *event) {return false;}
    virtual bool keyReleaseEvent(QKeyEvent *event) {return false;};
    virtual bool mouseDoubleClickEvent(QMouseEvent *event) {return false;};
    virtual bool mousePressEvent(QMouseEvent *event) {return false;};
    virtual bool mouseMoveEvent(QMouseEvent *event) {return false;};
    virtual bool mouseReleaseEvent(QMouseEvent *event) {return false;};

private:
    inline void setScene(QGraphicsScene *scene) {m_scene = scene;};
    inline void setMap(InteractiveMap *map) {m_map = map;};
    bool handleKeyPressEvent(QKeyEvent *event);
    bool handleKeyReleaseEvent(QKeyEvent *event);
    bool handleMousePressEvent(QMouseEvent *event);
    bool handleMouseMoveEvent(QMouseEvent *event);
    bool handleMouseReleaseEvent(QMouseEvent *event);
    bool handleMouseDoubleClickEvent(QMouseEvent *event);

protected:
    QGraphicsScene *m_scene;
    InteractiveMap *m_map;
    OperatorMode m_mode = CreateEdit;  ///< 操作模式

private:
    bool m_mouseMoveTrackingEnable = false; ///< 即便没有按下鼠标也能出发move事件
    bool m_skipOnceKeyEvent = false;     ///< 跳过一次release直到新的press
    bool m_skipOnceMouseEvent = false;   ///< 跳过多次move和一次release直到新的press
    bool m_keyEventEnable = false;       ///< 当突然被切换到该操作器时，防止前一个操作器的release事件没有处理
    bool m_mouseEventEnable = false;     ///< 当突然被切换到该操作器时，防止前一个操作器的release事件没有处理
    bool m_mouseMoveEventEnable = false; ///< press和doubleClick按下后触发move事件
};

#endif // INTERACTIVEMAP_H
