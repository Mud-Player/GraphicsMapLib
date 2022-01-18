﻿#ifndef MAPOBJECTITEM_H
#define MAPOBJECTITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsPixmapItem>
#include <QGeoCoordinate>

/*!
 * \brief 地图对象
 * \details 可用于将图标添加到地图上
 * \note 1.该对象不受地图缩放影响，将会始终保持屏幕大小
 * 2.默认会阻断鼠标时间穿透到地图上，但是要想穿透鼠标事件请调用setAllowMouseEvent(false)
 * 3.pressed信号总是可以收到
 */
class GRAPHICSMAPLIB_EXPORT MapObjectItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    MapObjectItem(const QGeoCoordinate &coord = {0, 0, 0});
    ~MapObjectItem();
    /// 设置经纬度位置
    void setCoordinate(const QGeoCoordinate &coord);
    /// 获取当前经纬度位置
    const QGeoCoordinate &coordinate() const;
    /// 设置图标，无效资源将使用默认图标
    void setIcon(const QPixmap &pixmap);
    /// 设置图标为纯色，传QColor()可以取消纯色 note: 高分辨图片对该接口性能影响较大
    /// \bug QGraphicsView使用OpenGL窗口时，第二个实例调用该函数会变成黑框
    void setIconColor(const QColor &color, qreal strength = 1.0);
    /// 设置文字
    void setText(const QString &text, Qt::Alignment align = Qt::AlignCenter);
    /// 设置文字颜色
    void setTextColor(const QColor &color);
    /// 设置是否允许鼠标事件，影响是否能够像QAbstractButton一样触发点击信号(但是可以收到press信号)
    void setAllowMouseEvent(bool enable);
    /// 设置鼠标可拖拽
    void setMovable(bool movable);
    /// 设置选中性
    void setCheckable(bool checkable);
    /// 设置为选中(不会触发信号)
    void setChecked(bool checked);
    /// 是否选中
    bool isChecked() const;

public:
    /// 获取所有的实例
    static const QSet<MapObjectItem*> &items();

signals:
    void clicked(bool checked = false);
    void pressed();
    void released();
    void toggled(bool checked);
    void coordinateChanged(const QGeoCoordinate &coord);
    void rotationChanged(const qreal &degree);

protected:
    /// 获取rotation信号和移动信号
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    static QSet<MapObjectItem*> m_items;         ///< 所有实例

private:
    QGeoCoordinate          m_coord;
    QGraphicsEllipseItem    m_border;
    QGraphicsSimpleTextItem m_text;
    //
    bool m_enableMouse = true;
    bool m_checkable = false;
    bool m_checked = false;
    //
    QPoint m_pressPos;
};

#endif // MAPOBJECTITEM_H
