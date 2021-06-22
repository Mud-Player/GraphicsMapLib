#ifndef MAPOBJECTITEM_H
#define MAPOBJECTITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsPixmapItem>
#include <QGeoCoordinate>

/*!
 * \brief 地图对象
 * \details 可用于将图标添加到地图上
 * \note 该对象不受地图缩放影响，将会始终保持屏幕大小
 */
class GRAPHICSMAPLIB_EXPORT MapObjectItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    MapObjectItem();
    ~MapObjectItem();
    /// 设置经纬度位置
    void setCoordinate(const QGeoCoordinate &coord);
    /// 获取当前经纬度位置
    const QGeoCoordinate &coordinate() const;
    /// 设置图标，无效资源将使用默认图标
    void setIcon(const QString &url);
    /// 设置图标为纯色，传QColor()可以取消纯色 note: 高分辨图片对该接口性能影响较大
    void setIconColor(const QColor &color, qreal strength = 1.0);
    /// 设置文字
    void setText(const QString &text, Qt::Alignment align = Qt::AlignCenter);
    /// 设置文字颜色
    void setTextColor(const QColor &color);
    /// 设置鼠标可拖拽
    void setMovable(bool movable);

public:
    /// 获取所有的实例
    static const QSet<MapObjectItem*> &items();

signals:
    void coordinateChanged(const QGeoCoordinate &coord);
    void rotationChanged(const qreal &degree);

protected:
    /// 获取rotation信号和移动信号
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

private:
    static QSet<MapObjectItem*> m_items;         ///< 所有实例

private:
    QGeoCoordinate          m_coord;
    QGraphicsSimpleTextItem m_text;
};

#endif // MAPOBJECTITEM_H
