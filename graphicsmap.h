﻿#ifndef GRAPHICSMAP_H
#define GRAPHICSMAP_H

#include "GraphicsMapLib_global.h"
#include <QWidget>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QCache>
#include <QGeoCoordinate>

class GraphicsMapThread;
/*!
 * \brief 基于Graphics View的地图
 * \details 其仅用于显示瓦片地图，要实现地图以外的功能可以继承该类
 * \note 鼠标拖拽地图可通过setDragMode(QGraphicsView::ScrollHandDrag)实现
 */
class GRAPHICSMAPLIB_EXPORT GraphicsMap : public QGraphicsView
{
    Q_OBJECT

public:
    /// 瓦片参数描述结构体
    struct TileSpec {
        quint8 type;  ///< 瓦片类型（用于标识不同路劲资源的地图)
        quint8 zoom;  ///< 瓦片层级
        quint32 x;    ///< 瓦片X轴编号
        quint32 y;    ///< 瓦片Y轴编号
        inline TileSpec rise() const {
            return GraphicsMap::TileSpec({type, static_cast<quint8>(zoom-1), x/2, y/2});
        }
        inline bool operator< (const TileSpec &rhs) const {
            return this->toLong() < rhs.toLong();
        };
        inline bool operator== (const TileSpec &rhs) const {
            return this->toLong() == rhs.toLong();
        };
        inline qlonglong toLong() const {
            return (qlonglong(type)<<52) | (qlonglong(zoom)<< 44) | (qlonglong(x)<< 22) | y;
        };
    };

    GraphicsMap(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~GraphicsMap();
    /// 设置瓦片路径
    void setTilePath(const QString &path);
    /// 设置缩放等级
    void setZoomLevel(const float &zoom);
    const float &zoomLevel() const;
    /// 设置瓦片缓存数量
    void setTileCacheCount(const int &count);
    /// 设置是否反转Y轴瓦片编号(标准下载的瓦片Y轴编号都是自上而下增加，不过某些情况可能是反的)
    void setYInverted(const bool &isInverted);
    /// 居中
    void centerOn(const QGeoCoordinate &coord);
    /// 获取窗口坐标对应的经纬度
    QGeoCoordinate toCoordinate(const QPoint &point) const;
    /// 获取经纬度对应的窗口坐标
    QPoint toPoint(const QGeoCoordinate &coord) const;

public:
    /// 获取场景坐标对应的经纬度
    static QGeoCoordinate toCoordinate(const QPointF &point);
    /// 获取经纬度对应的场景坐标
    static QPointF toScene(const QGeoCoordinate &coord);
    /// 通过资源路径，获取唯一对应的资源类型
    static quint8 mapType(const QString &path);

signals:
    void tileRequested(const GraphicsMap::TileSpec &topLeft, const GraphicsMap::TileSpec &bottomRight);
    void pathRequested(const QString &path);

private:
    void updateTile();

private:
    static QStringList m_mapTypes; ///< 资源路径类型
private:
    GraphicsMapThread    *m_mapThread;
    QSet<QGraphicsItem*> m_tiles;
    quint8               m_type;           ///< 瓦片资源类型
    //
    bool m_isloading;
    bool m_hasPendingLoad;
    float m_zoom;
};

inline uint qHash(const GraphicsMap::TileSpec &key, uint seed)
{
    qlonglong keyVal = (qlonglong(key.zoom)<<48) + (qlonglong(key.x)<< 24) + key.y;
    return qHash(keyVal, seed);
}

/*!
 * \brief 瓦片地图管理线程
 * \details 负责加载瓦片、卸载瓦片
 */
class GraphicsMapThread : public QObject
{
    Q_OBJECT

    /// 瓦片缓存节点，配合QCache实现缓存机制
    struct TileCacheNode {
        GraphicsMap::TileSpec tileSpec;
        QGraphicsItem *value = nullptr;
        ~TileCacheNode();
    };

public:
    GraphicsMapThread();
    ~GraphicsMapThread();


public slots:
    /// 请求刷新瓦片区域
    void requestTile(const GraphicsMap::TileSpec &topLeft, const GraphicsMap::TileSpec &bottomRight);
    /// 请求更改瓦片资源来源
    void requestPath(const QString &path);

public:
    /// 设置瓦片缓存数量
    void setTileCacheCount(const int &count);
    /// 设置是否Y轴瓦片编号(标准下载的瓦片Y轴编号都是自上而下增加，不过某些情况可能是反的)
    void setYInverted(const bool &isInverted);

signals:
    void tileToAdd(QGraphicsItem *tile);
    void tileToRemove(QGraphicsItem *tile);
    void requestFinished();

private:
    void showItem(const GraphicsMap::TileSpec &tileSpec);
    void hideItem(const GraphicsMap::TileSpec &tileSpec);
    /// 从磁盘加载瓦片文件
    QGraphicsPixmapItem* loadTileItem(const GraphicsMap::TileSpec &tileSpec);
    void createAscendingTileCache(const GraphicsMap::TileSpec &tileSpec, QSet<GraphicsMap::TileSpec> &sets);

private:
    QCache<GraphicsMap::TileSpec, TileCacheNode> m_tileCache; ///<已加载瓦片缓存
    QSet<GraphicsMap::TileSpec>    m_tileTriedToShowdSet;     ///<已尝试显示瓦片编号集合(上一次调用过showItem的所有瓦片，存在依赖关系的瓦片，实际上只有顶层才显示)
    QSet<GraphicsMap::TileSpec>    m_tileShowedSet;           ///<实际显示瓦片编号集合
    //
    GraphicsMap::TileSpec m_preTopLeft;
    GraphicsMap::TileSpec m_preBottomRight;
    //
    QString          m_path;
    bool             m_yInverted;
};

#endif // GRAPHICSMAP_H
