#include "graphicsmap.h"
#include <QScrollBar>
#include <QOpenGLWidget>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QDebug>
#include <QGraphicsLineItem>
#include <QtMath>
#include <QThread>
#include <QFileInfo>
#include <QtMath>

#define ZOOM_BASE 10  ///< ZOOM_BASE级瓦片正好缩放为原比例(1:1),低于ZOOM_BASE级的放大，反之缩小
#define TILE_LEN 256  ///< 瓦片长度，标准的都是256 * 256
#define SCENE_LEN ((1<<ZOOM_BASE) * TILE_LEN)   ///< 存放瓦片的场景大小

GraphicsMap::TileSpec GraphicsMap::TileSpec::rise() const
{
    return GraphicsMap::TileSpec({this->zoom-1, this->x/2, this->y/2});
}

bool GraphicsMap::TileSpec::operator <(const GraphicsMap::TileSpec &rhs) const
{
    qlonglong lhsVal = (qlonglong(this->zoom)<<48) + (qlonglong(this->x)<< 24) + this->y;
    qlonglong rhsVal = (qlonglong(rhs.zoom)<<48) + (qlonglong(rhs.x)<< 24) + rhs.y;
    return  lhsVal < rhsVal;
}

bool GraphicsMap::TileSpec::operator==(const GraphicsMap::TileSpec &rhs) const
{
    return (this->zoom == rhs.zoom) && (this->x == rhs.x) && (this->y == rhs.y);
}

GraphicsMap::GraphicsMap(QGraphicsScene *scene) : QGraphicsView(scene),
    m_isloading(false),
    m_hasPendingLoad(false),
    m_zoom(1)
{
    qRegisterMetaType<GraphicsMap::TileSpec>("GraphicsMap::TileSpec");

    // connect those necessary slot for map tile loading
    m_mapThread = new GraphicsMapThread;
    connect(this, &GraphicsMap::tileRequested, m_mapThread, &GraphicsMapThread::requestTile, Qt::QueuedConnection);
    connect(m_mapThread, &GraphicsMapThread::tileToAdd, this, [&](QGraphicsItem* item){
        this->scene()->addItem(item);
        m_tiles.insert(item);
    }, Qt::QueuedConnection);
    connect(m_mapThread, &GraphicsMapThread::tileToRemove, this, [&](QGraphicsItem* item){
        this->scene()->removeItem(item);
        m_tiles.remove(item);
    }, Qt::QueuedConnection);
    connect(m_mapThread, &GraphicsMapThread::requestFinished, this, [&](){
        m_isloading = false;
        if(m_hasPendingLoad) {
            updateTile();
            m_hasPendingLoad = false;
        }
    }, Qt::QueuedConnection);
    // TODO: We have to use Qt::QueuedConnection, if not, we will see the map twinkle when scale
    connect(this->horizontalScrollBar(), &QScrollBar::valueChanged, this, [&](){
    if(m_isloading)
        m_hasPendingLoad = true;
    else
        updateTile();
    }, Qt::QueuedConnection);
    //
    this->scene()->setSceneRect(-SCENE_LEN/2, -SCENE_LEN/2, SCENE_LEN, SCENE_LEN);
    setZoomLevel(2);
}

GraphicsMap::~GraphicsMap()
{
    // 在此处从场景移出瓦片，防止和多线程析构冲突
    for(auto item : m_tiles) {
        this->scene()->removeItem(item);
    }
    delete m_mapThread;
}

void GraphicsMap::setTilePath(const QString &path)
{
    m_mapThread->setTilePath(path);
}

void GraphicsMap::setZoomLevel(const float &zoom)
{
    m_zoom = qBound(0.0f, zoom, 20.0f);
    auto zoomLevelDiff = m_zoom - ZOOM_BASE;
    auto scaleValue = qPow(2, zoomLevelDiff);
    setTransform(QTransform::fromScale(scaleValue, scaleValue));
    //
    if(m_isloading)
        m_hasPendingLoad = true;
    else
        updateTile();
}

const float &GraphicsMap::zoomLevel() const
{
    return m_zoom;
}

void GraphicsMap::setTileCacheCount(const int &count)
{
    m_mapThread->setTileCacheCount(count);
}

void GraphicsMap::setYInverted(const bool &isInverted)
{
    m_mapThread->setYInverted(isInverted);
}

void GraphicsMap::centerOn(const QGeoCoordinate &coord)
{
    auto pos = fromCoordinate(coord);
    QGraphicsView::centerOn(pos);
}

/// \see https://blog.csdn.net/iispring/article/details/8565177
/// R = SCENE_LEN / 2PI
QGeoCoordinate GraphicsMap::toCoordinate(const QPoint &point) const
{
    /// NOTE: R = SCENE_LEN / 2PI
    auto scenePos = this->mapToScene(point);
    return toCoordinate(scenePos);
}

QGeoCoordinate GraphicsMap::toCoordinate(const QPointF &point)
{
    auto radLon = point.x() * 2 * M_PI/ SCENE_LEN;
    auto radLat = 2 * qAtan(qPow(M_E, 2*M_PI*point.y()/SCENE_LEN)) - M_PI_2;
    return  {qRadiansToDegrees(radLon), qRadiansToDegrees(radLat)};
}

/// \see https://blog.csdn.net/iispring/article/details/8565177
/// R = SCENE_LEN / 2PI
QPointF GraphicsMap::fromCoordinate(const QGeoCoordinate &coord)
{
    /// NOTE: R = SCENE_LEN / 2PI
    auto radLon = qDegreesToRadians(coord.longitude());
    auto radLat = qDegreesToRadians(coord.latitude());
    auto x = SCENE_LEN * radLon / 2.0 / M_PI;
    auto y = SCENE_LEN / 2.0 / M_PI * qLn( qTan(M_PI_4+radLat/2.0) );
    return {x, -y}; // NOTO: as for Qt, it's y asscending from up to bottom
}

void GraphicsMap::updateTile()
{
    int intZoom = qFloor(m_zoom+0.5);
    //
    int tileCount = qPow(2, intZoom);
    auto topLeftPos = mapToScene(viewport()->geometry().topLeft());
    auto bottomRightPos = mapToScene(viewport()->geometry().bottomRight());
    int xBegin = (topLeftPos.x()+SCENE_LEN/2) / SCENE_LEN * tileCount - 1;
    int yBegin = (topLeftPos.y()+SCENE_LEN/2) / SCENE_LEN * tileCount - 1;
    int xEnd = (bottomRightPos.x()+SCENE_LEN/2) / SCENE_LEN * tileCount + 1;
    int yEnd = (bottomRightPos.y()+SCENE_LEN/2) / SCENE_LEN * tileCount + 1;
    if(xBegin < 0) xBegin = 0;
    if(yBegin < 0) yBegin = 0;
    if(xEnd >= tileCount) xEnd = tileCount - 1;
    if(yEnd >= tileCount) yEnd = tileCount - 1;
    m_isloading = true;
    emit tileRequested({intZoom, xBegin, yBegin}, {intZoom, xEnd, yEnd});

}

GraphicsMapThread::TileCacheNode::~TileCacheNode()
{
    delete value;
}

GraphicsMapThread::GraphicsMapThread() :
    m_preTopLeft({0, 0, 0}),
    m_preBottomRight({0, 0, 0}),
    m_yInverted(false)
{
    m_tileCache.setMaxCost(1000);
    //
    QThread *thread = new QThread;
    thread->setObjectName("MapThread");
    this->moveToThread(thread);
    thread->start();
}

GraphicsMapThread::~GraphicsMapThread()
{
    this->thread()->quit();
    this->thread()->wait();
    delete this->thread();
}


void GraphicsMapThread::requestTile(const GraphicsMap::TileSpec &topLeft, const GraphicsMap::TileSpec &bottomRight)
{
    if(m_preTopLeft == topLeft && m_preBottomRight == bottomRight) {
        emit requestFinished();
        return;
    }

    // y向下递增
    QSet<GraphicsMap::TileSpec> curViewSet;
    QSet<GraphicsMap::TileSpec> preViewSet;
    {
        const int &zoom = topLeft.zoom;
        const int xBegin = topLeft.x;
        const int xEnd = bottomRight.x;
        const int yBegin = topLeft.y;
        const int yEnd = bottomRight.y;
        for(int x = xBegin; x <= xEnd; ++x) {
            for(int y = yBegin; y <= yEnd; ++y) {
                curViewSet.insert({zoom, x, y});
            }
        }
    }
    {
        const int &zoom = m_preTopLeft.zoom;
        const int xBegin = m_preTopLeft.x;
        const int xEnd = m_preBottomRight.x;
        const int yBegin = m_preTopLeft.y;
        const int yEnd = m_preBottomRight.y;
        for(int x = xBegin; x <= xEnd; ++x) {
            for(int y = yBegin; y <= yEnd; ++y) {
                preViewSet.insert({zoom, x, y});
            }
        }
    }

    m_preTopLeft = topLeft;
    m_preBottomRight = bottomRight;

    // compute which to load and which to unload
    QSet<GraphicsMap::TileSpec> needToShowTileSet;
    QSet<GraphicsMap::TileSpec> needToHideTileSet;
    {
        QSetIterator<GraphicsMap::TileSpec> iter(curViewSet);
        while (iter.hasNext()) {
            auto tileSpec = iter.next();
            createAscendingTileCache(tileSpec, needToShowTileSet);
        }
    }
    {
        QSetIterator<GraphicsMap::TileSpec> iter(preViewSet);
        while (iter.hasNext()) {
            auto tileSpec = iter.next();
            createAscendingTileCache(tileSpec, needToHideTileSet);
        }
    }
    QSet<GraphicsMap::TileSpec> realToHideTileSet = needToHideTileSet - needToShowTileSet;

    // update the scene tiles
    {
        QSetIterator<GraphicsMap::TileSpec> iter(needToShowTileSet);
        while (iter.hasNext()) {
            auto &tileSpec = iter.next();
            showItem(tileSpec);
        }
    }
    {
        QSetIterator<GraphicsMap::TileSpec> iter(realToHideTileSet);
        while (iter.hasNext()) {
            auto &tileSpec = iter.next();
            hideItem(tileSpec);
        }
    }

    emit requestFinished();
}

void GraphicsMapThread::setTilePath(const QString &path)
{
    m_path = path;
}

void GraphicsMapThread::setTileCacheCount(const int &count)
{
    m_tileCache.setMaxCost(count);
}

void GraphicsMapThread::setYInverted(const bool &isInverted)
{
    m_yInverted = isInverted;
}

void GraphicsMapThread::showItem(const GraphicsMap::TileSpec &tileSpec)
{
    if(m_tileShowedSet.contains(tileSpec))
        return;

    auto tileItem = m_tileCache.object(tileSpec);
    if(tileItem->value) {
        emit tileToAdd(tileItem->value);
        m_tileShowedSet.insert(tileSpec);
    }
}

void GraphicsMapThread::hideItem(const GraphicsMap::TileSpec &tileSpec)
{
    // 看不见的直接不管
    if(!m_tileShowedSet.contains(tileSpec))
        return;

    auto tileItem = m_tileCache.object(tileSpec);
    if(tileItem->value) {
        emit tileToRemove(tileItem->value);
        m_tileShowedSet.remove(tileSpec);
    }
}

/*!
 * \brief MudMapThread::loadTileItem
 * \note QTransform的srt顺序对结果有影响，并且和三维矩阵用法不一样，
 * 在该函数实现中，先scale再translate，可以理解成将瓦片按照原始大小排列在矩形中(比如1层有四张瓦片，那么排列在256*4->256*4的矩形中)，
 * 然后这个矩形从右下角整个向左上角缩放达到和sceneRect()正好重合，以实现所有不同zoom的瓦片都重叠在sceneRect()上，也达到了缺省瓦片通过上层瓦片显示的效果。
 * 为了方便经纬度和场景坐标的转换，这里将经纬度（0，0）映射在了场景坐标的（0，0）处，所以注意xOff和yOff是先移动到以（0，0）为原点的位置，再向左上移动半个场景的宽度和高度
 */
QGraphicsPixmapItem *GraphicsMapThread::loadTileItem(const GraphicsMap::TileSpec &tileSpec)
{
    int tileCount = qPow(2, tileSpec.zoom);
    //
    QString fileName = QString("%1/%2/%3/%4.jpg")
            .arg(m_path)
            .arg(tileSpec.zoom)
            .arg(tileSpec.x)
            .arg(m_yInverted ? tileCount - tileSpec.y - 1 : tileSpec.y);
    if(!QFileInfo::exists(fileName))
        return nullptr;

    auto tileItem = new QGraphicsPixmapItem(fileName);
    tileItem->setZValue(tileSpec.zoom - 20);

    //
    double xOff = TILE_LEN * (tileSpec.x  - tileCount/2.0);     // see also: TILE_LEN * tileSpec.x - (TILE_LEN*tileCount) / 2;
    double yOff = TILE_LEN * (tileSpec.y  - tileCount/2.0);     // see also: TILE_LEN * tileSpec.y - (TILE_LEN*tileCount) / 2;
    double scaleFac = 1.0 / qPow(2, (tileSpec.zoom-ZOOM_BASE));
    QTransform transform;
    transform.scale(scaleFac, scaleFac)
            .translate(xOff, yOff);
    tileItem->setTransform(transform);
    return tileItem;
}

void GraphicsMapThread::createAscendingTileCache(const GraphicsMap::TileSpec &tileSpec, QSet<GraphicsMap::TileSpec> &sets)
{
    auto tileCacheItem = m_tileCache.object(tileSpec);

    //
    if(!tileCacheItem) {
        tileCacheItem = new GraphicsMapThread::TileCacheNode;
        tileCacheItem->value = loadTileItem(tileSpec);
        tileCacheItem->tileSpec = tileSpec;
        m_tileCache.insert(tileSpec, tileCacheItem);
    }
    sets.insert(tileSpec);

    if(!tileCacheItem->value && tileSpec.zoom != 0)
        createAscendingTileCache(tileSpec.rise(), sets);
}
