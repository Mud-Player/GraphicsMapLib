#ifndef MAPOPERATOR_H
#define MAPOPERATOR_H

#include "interactivemap.h"
class MapOperator;
class MapEllipseItem;
class MapPolygonItem;
class MapRouteItem;
class MapObjectItem;
class MapRangeRingItem;
class MapTrailItem;
class MapLineItem;
class MapRectItem;
class MapTableItem;

/*!
 * \brief 圆形创建操作器
 * \details 左键双击完成创建
 */
class GRAPHICSMAPLIB_EXPORT MapEllipseOperator : public MapOperator
{
    Q_OBJECT

public:
    MapEllipseOperator(QObject *parent = nullptr);

    /// 接管已经创建的圆形(请确保被应用为操作器后再调用)
    void takeOver(MapEllipseItem *item);
signals:
    void created(MapEllipseItem *item);
    void deleted(MapEllipseItem *item);
protected:
    virtual void ready() override;
    virtual void end() override;
    virtual bool keyPressEvent(QKeyEvent *event) override;
    virtual bool mouseDoubleClickEvent(QMouseEvent *) override;
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool mouseMoveEvent(QMouseEvent *event) override;

private:
    void detach();
private:
    QGeoCoordinate  m_first;
    MapEllipseItem *m_ellipse;
};

/*!
 * \brief 多边形创建操作器
 * \details 左键双击结束编辑
 */
class GRAPHICSMAPLIB_EXPORT MapPolygonOperator : public MapOperator
{
    Q_OBJECT

public:
    MapPolygonOperator(QObject *parent = nullptr);

    /// 接管已经创建的多边行(请确保被应用为操作器后再调用)
    void takeOver(MapPolygonItem *item);
signals:
    void created(MapPolygonItem *item);
    void deleted(MapPolygonItem *item);
protected:
    virtual void ready() override;
    virtual void end() override;
    virtual bool keyPressEvent(QKeyEvent *event) override;
    virtual bool mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;

private:
    void detach();
private:
    QPoint         m_pressPos;
    MapPolygonItem *m_polygon;
};

/*!
 * \brief 图标对象创建操作器
 * \details 左键单击完成单图标对象的创建，右键双击结束编辑
 */
class GRAPHICSMAPLIB_EXPORT MapObjectOperator : public MapOperator
{
    Q_OBJECT

public:
    MapObjectOperator(QObject *parent = nullptr);

    /// 接管已经创建的对象
    void takeOver(MapObjectItem *item);

signals:
    void created(MapObjectItem *item);
    void deleted(MapObjectItem *item);
protected:
    virtual void ready() override;
    virtual void end() override;
    virtual bool keyPressEvent(QKeyEvent *event) override;
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void detach();

private:
    QPoint         m_pressPos;
    MapObjectItem *m_obj = nullptr;
};

/*!
 * \brief 图标对象标签创建操作器
 * \details
 */
class GRAPHICSMAPLIB_EXPORT MapScutcheonOperator : public MapOperator
{
    Q_OBJECT

public:
    MapScutcheonOperator(QObject *parent = nullptr);

    /// 接管已经创建的对象
    void takeOver(MapTableItem *item);
    /// 设置是否忽略鼠标事件
    void setIgnoreMouseEvent(bool bIgnore);

    void setOffset(const QPoint &offset);

    QPoint GetOffset() const {return m_Offset; }
signals:
    void changeOffset();
protected:
    virtual void ready() override;
    virtual void end() override;
    virtual bool keyPressEvent(QKeyEvent *event) override;
    virtual bool mouseMoveEvent(QMouseEvent * event) override;
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool mouseDoubleClickEvent(QMouseEvent *event) override;
private:
    bool                    m_bMousePress;
    QPoint                  m_PressPosition;
    QPoint                  m_Offset;
    MapTableItem            *m_toolTip;
    bool                    m_bIgnoreEvent; // 鼠标忽略
};



/*!
 * \brief 航路创建操作器
 * \details 双击和右键完成单航路的创建，多次点击实现新增航点
 */
class GRAPHICSMAPLIB_EXPORT MapRouteOperator : public MapOperator
{
    Q_OBJECT

public:
    MapRouteOperator(QObject *parent = nullptr);

    /// 接管已经创建的航路(请确保被应用为操作器后再调用)
    void takeOver(MapRouteItem *item);

    /// 设置航点的默认图标
    void setWaypointIcon(const QPixmap &pixmap);

signals:
    ///< 创建信号
    void created(MapRouteItem *item);
    void deleted(MapRouteItem *item);
protected:
    virtual void ready() override;
    virtual void end() override;
    virtual bool keyPressEvent(QKeyEvent *event) override;
    virtual bool mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPoint   m_pressPos;
    //
    MapRouteItem  *m_route = nullptr;
    //
    QPixmap  m_waypointIcon;
};


/*!
 * \brief 测距线段操作器
 * \details 完成测距线段的创建及修改
 */
class GRAPHICSMAPLIB_EXPORT MapRangeLineOperator : public MapOperator
{
	Q_OBJECT

public:
	MapRangeLineOperator(QObject *parent = nullptr);

signals:
	///< 创建信号
	void created(MapLineItem *item);
    void deleted(MapLineItem *item);
protected:
	virtual void ready() override;
	virtual void end() override;
    virtual bool keyPressEvent(QKeyEvent *event) override;
	virtual bool mousePressEvent(QMouseEvent *event) override;
	virtual bool mouseReleaseEvent(QMouseEvent *event) override;
	virtual bool mouseMoveEvent(QMouseEvent *event) override;

private:
	QPoint       m_pressEndPos;
	QPoint       m_pressFirstPos;
	//
	MapLineItem  *m_line;
};

/*!
 * \brief 矩形创建操作器
 * \details 左键双击完成编辑
 */
class GRAPHICSMAPLIB_EXPORT MapRectOperator : public MapOperator
{
    Q_OBJECT

public:
    MapRectOperator(QObject *parent = nullptr);

    /// 接管已经创建的矩形(请确保被应用为操作器后再调用)
    void takeOver(MapRectItem *item);

signals:
    void created(MapRectItem *item);
    void deleted(MapRectItem *item);
protected:
    virtual void ready() override;
    virtual void end() override;
    virtual bool keyPressEvent(QKeyEvent *event) override;
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool mouseMoveEvent(QMouseEvent *event) override;

private:
    void detach();

private:
    bool            m_ignoreEvent;
    QGeoCoordinate  m_first;
    MapRectItem *m_rect;
};


#endif // MAPOPERATOR_H
