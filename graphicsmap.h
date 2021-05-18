#ifndef GRAPHICSMAP_H
#define GRAPHICSMAP_H

#include "GraphicsMapLib_global.h"
#include <QWidget>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QCache>
#include <QGeoCoordinate>

class MudMapThread;
/*!
 * \brief 基于Graphics View的地图
 * \details 其仅用于显示瓦片地图，要实现地图以外的功能可以继承该类
 */
class GRAPHICSMAPLIB_EXPORT MudMap : public QGraphicsView
{
    Q_OBJECT

public:
    /// 瓦片参数描述结构体
    struct TileSpec {
        int zoom;
        int x;
        int y;
        TileSpec rise() const;
        bool operator< (const TileSpec &rhs) const;
        bool operator== (const TileSpec &rhs) const;
    };

    MudMap(QGraphicsScene *scene);
    ~MudMap();
    /// 设置瓦片路径
    void setTilePath(const QString &path);
    /// 设置缩放等级
    void setZoomLevel(const float &zoom);
    /// 设置瓦片缓存数量
    void setTileCacheCount(const int &count);
    /// 设置是否反转Y轴瓦片编号(标准下载的瓦片Y轴编号都是自上而下增加，不过某些情况可能是反的)
    void setYInverted(const bool &isInverted);
    /// 获取窗口坐标对应的经纬度
    QGeoCoordinate toCoordinate(const QPoint &point) const;
    /// 居中
    void centerOn(const QGeoCoordinate &coord);

public:
    /// 获取场景坐标对应的经纬度
    static QGeoCoordinate toCoordinate(const QPointF &point);
    /// 获取经纬度对应的场景坐标
    static QPointF fromCoordinate(const QGeoCoordinate &coord);


signals:
    void tileRequested(const MudMap::TileSpec &topLeft, const MudMap::TileSpec &bottomRight);

protected:
    virtual void wheelEvent(QWheelEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

private:
    void updateTile();

private:
    MudMapThread        *m_mapThread;
    QSet<QGraphicsItem*> m_tiles;
    //
    bool m_isloading;
    bool m_hasPendingLoad;
    float m_zoom;
};

inline uint qHash(const MudMap::TileSpec &key, uint seed)
{
    qlonglong keyVal = (qlonglong(key.zoom)<<48) + (qlonglong(key.x)<< 24) + key.y;
    return qHash(keyVal, seed);
}

/*!
 * \brief 瓦片地图管理线程
 * \details 负责加载瓦片、卸载瓦片
 */
class MudMapThread : public QObject
{
    Q_OBJECT
    /// 瓦片缓存节点，配合QCache实现缓存机制
    struct TileCacheNode {
        MudMap::TileSpec tileSpec;
        QGraphicsItem *value = nullptr;
        ~TileCacheNode();
    };

public:
    MudMapThread();
    ~MudMapThread();
public slots:
    void requestTile(const MudMap::TileSpec &topLeft, const MudMap::TileSpec &bottomRight);
    /// 设置瓦片路径
    void setTilePath(const QString &path);
    /// 设置瓦片缓存数量
    void setTileCacheCount(const int &count);
    /// 设置是否Y轴瓦片编号(标准下载的瓦片Y轴编号都是自上而下增加，不过某些情况可能是反的)
    void setYInverted(const bool &isInverted);

signals:
    void tileToAdd(QGraphicsItem *tile);
    void tileToRemove(QGraphicsItem *tile);
    void requestFinished();

private:
    void showItem(const MudMap::TileSpec &tileSpec);
    void hideItem(const MudMap::TileSpec &tileSpec);
    /// 从磁盘加载瓦片文件
    QGraphicsPixmapItem* loadTileItem(const MudMap::TileSpec &tileSpec);
    void createAscendingTileCache(const MudMap::TileSpec &tileSpec, QSet<MudMap::TileSpec> &sets);

private:
    QCache<MudMap::TileSpec, TileCacheNode> m_tileCache; ///<已加载瓦片缓存
    QSet<MudMap::TileSpec>    m_tileShowedSet;           ///<已显示瓦片编号集合(存在依赖关系的瓦片，实际上只有顶层才显示，但是子瓦片仍然被当作Showed)
    //
    MudMap::TileSpec m_preTopLeft;
    MudMap::TileSpec m_preBottomRight;
    QString          m_path;
    bool             m_yInverted;
};

#endif // GRAPHICSMAP_H
