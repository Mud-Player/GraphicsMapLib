#ifndef MAPOPERATOR_H
#define MAPOPERATOR_H

#include "interactivemap.h"
class InteractiveMapOperator;
class MapEllipseItem;
class MapPolygonItem;
class MapRouteItem;
class MapObjectItem;
class MapRangeRingItem;
class MapTrailItem;

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
 * \details 双击和右键完成单图标对象的创建，多次点击实现图标初始初始位置与移动路线的规划
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

/*!
 * \brief 航路创建操作器
 * \details 双击和右键完成单航路的创建，多次点击实现新增航点
 */
class GRAPHICSMAPLIB_EXPORT MapRouteOperator : public InteractiveMapOperator
{
    Q_OBJECT

public:
    MapRouteOperator(QObject *parent = nullptr);

signals:
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
    MapRouteItem  *m_route;
};

#endif // MAPOPERATOR_H
