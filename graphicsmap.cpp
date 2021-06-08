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

QStringList GraphicsMap::m_mapTypes;

GraphicsMap::GraphicsMap(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent),
    m_type(0),
    m_isloading(false),
    m_hasPendingLoad(false),
    m_zoom(1),
    m_minZoom(1),
    m_maxZoom(20)
{
    qRegisterMetaType<GraphicsMap::TileSpec>("GraphicsMap::TileSpec");

    init();
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
    m_type = mapType(path);
    emit pathRequested(path);
    updateTile();
}

void GraphicsMap::setZoomLevel(const float &zoom)
{
    if(m_zoom == zoom)
        return;
    m_zoom = qBound(m_minZoom, zoom, m_maxZoom);
    auto zoomLevelDiff = m_zoom - ZOOM_BASE;
    auto scaleValue = qPow(2, zoomLevelDiff);
    setTransform(QTransform::fromScale(scaleValue, scaleValue));
    //
    if(m_isloading)
        m_hasPendingLoad = true;
    else
        updateTile();
    emit zoomChanged(m_zoom);
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
    auto pos = toScene(coord);
    QGraphicsView::centerOn(pos);
}

QGeoCoordinate GraphicsMap::toCoordinate(const QPoint &point) const
{
    auto scenePos = this->mapToScene(point);
    return toCoordinate(scenePos);
}

QPoint GraphicsMap::toPoint(const QGeoCoordinate &coord) const
{
    auto scenePos = toScene(coord);
    return this->mapFromScene(scenePos);
}

/// \see https://blog.csdn.net/iispring/article/details/8565177
/// R = SCENE_LEN / 2PI
QGeoCoordinate GraphicsMap::toCoordinate(const QPointF &point)
{
    auto radLon = point.x() * 2 * M_PI/ SCENE_LEN;
    auto radLat = 2 * qAtan(qPow(M_E, 2*M_PI*point.y()/SCENE_LEN)) - M_PI_2;
    // NOTO: as for Qt, it's y asscending from up to bottom
    return  {qRadiansToDegrees(-radLat), qRadiansToDegrees(radLon)};
}

/// \see https://blog.csdn.net/iispring/article/details/8565177
/// R = SCENE_LEN / 2PI
QPointF GraphicsMap::toScene(const QGeoCoordinate &coord)
{
    /// NOTE: R = SCENE_LEN / 2PI
    double radLon = qDegreesToRadians(coord.longitude());
    double radLat = qDegreesToRadians(coord.latitude());
    double x = SCENE_LEN * radLon / 2.0 / M_PI;
    double y = SCENE_LEN / 2.0 / M_PI * qLn( qTan(M_PI_4+radLat/2.0) );
    // NOTO: as for Qt, it's y asscending from up to bottom
    return {x, -y};
}

/// 从1编号
quint8 GraphicsMap::mapType(const QString &path)
{
    if(!m_mapTypes.contains(path))
        m_mapTypes.append(path);
    return m_mapTypes.indexOf(path)+1;
}

void GraphicsMap::resizeEvent(QResizeEvent *event)
{
    double len = qMax(event->size().width(), event->size().height());
    auto zoom = log(len/SCENE_LEN) / log(2);
    m_minZoom = zoom + ZOOM_BASE;
    setZoomLevel(m_zoom);
}

void GraphicsMap::init()
{
    m_mapThread = new GraphicsMapThread;
    // connect those necessary slot for map tile loading
    connect(this, &GraphicsMap::tileRequested, m_mapThread, &GraphicsMapThread::requestTile, Qt::QueuedConnection);
    connect(this, &GraphicsMap::pathRequested, m_mapThread, &GraphicsMapThread::requestPath, Qt::QueuedConnection);
    //
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
    connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, [&](){
        if(m_isloading)
            m_hasPendingLoad = true;
        else
            updateTile();
    }, Qt::QueuedConnection);
}

void GraphicsMap::updateTile()
{
    quint8 intZoom = qFloor(m_zoom+0.5);
    //
    qint32 tileCount = qPow(2, intZoom);
    auto topLeftPos = mapToScene(viewport()->geometry().topLeft());
    auto bottomRightPos = mapToScene(viewport()->geometry().bottomRight());
    qint32 xBegin = (topLeftPos.x()+SCENE_LEN/2) / SCENE_LEN * tileCount - 1;
    qint32 yBegin = (topLeftPos.y()+SCENE_LEN/2) / SCENE_LEN * tileCount - 1;
    qint32 xEnd = (bottomRightPos.x()+SCENE_LEN/2) / SCENE_LEN * tileCount + 1;
    qint32 yEnd = (bottomRightPos.y()+SCENE_LEN/2) / SCENE_LEN * tileCount + 1;
    if(xBegin < 0) xBegin = 0;
    if(yBegin < 0) yBegin = 0;
    if(xEnd >= tileCount) xEnd = tileCount - 1;
    if(yEnd >= tileCount) yEnd = tileCount - 1;
    TileSpec topLeft{m_type, intZoom, static_cast<quint32>(xBegin), static_cast<quint32>(yBegin)};
    TileSpec bottomRight{m_type, intZoom, static_cast<quint32>(xEnd), static_cast<quint32>(yEnd)};
    if(m_preTopLeft == topLeft && m_preBottomRight == bottomRight)
        return;
    m_preTopLeft = topLeft;
    m_preBottomRight = bottomRight;
    m_isloading = true;
    emit tileRequested(topLeft, bottomRight);
}

GraphicsMapThread::TileCacheNode::~TileCacheNode()
{
    delete value;
}

GraphicsMapThread::GraphicsMapThread() :
    m_preTopLeft({0, 0, 0, 0}),
    m_preBottomRight({0, 0, 0, 0}),
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
    // hide all tile items if tile resource path is invalid
    if(m_path.isEmpty()) {
        for(auto &tile : m_tileShowedSet) {
            hideItem(tile);
        }
        emit requestFinished();
        return;
    }
    // just ignore the requeset if rect arec not changed
    if(m_preTopLeft == topLeft && m_preBottomRight == bottomRight) {
        emit requestFinished();
        return;
    }

    // y向下递增
    QSet<GraphicsMap::TileSpec> curViewSet;
    QSet<GraphicsMap::TileSpec> preViewSet;
    const auto &type = topLeft.type;
    {
        const auto &zoom = topLeft.zoom;
        const auto xBegin = topLeft.x;
        const auto xEnd = bottomRight.x;
        const auto yBegin = topLeft.y;
        const auto yEnd = bottomRight.y;
        for(auto x = xBegin; x <= xEnd; ++x) {
            for(auto y = yBegin; y <= yEnd; ++y) {
                curViewSet.insert({type, zoom, x, y});
            }
        }
    }
    {
        const auto &zoom = m_preTopLeft.zoom;
        const auto xBegin = m_preTopLeft.x;
        const auto xEnd = m_preBottomRight.x;
        const auto yBegin = m_preTopLeft.y;
        const auto yEnd = m_preBottomRight.y;
        for(auto x = xBegin; x <= xEnd; ++x) {
            for(auto y = yBegin; y <= yEnd; ++y) {
                preViewSet.insert({type, zoom, x, y});
            }
        }
    }

    m_preTopLeft = topLeft;
    m_preBottomRight = bottomRight;

    // compute which to load and which to unload
    QSet<GraphicsMap::TileSpec> needToHideTileSet = m_tileTriedToShowdSet;
    m_tileTriedToShowdSet.clear();
    {
        QSetIterator<GraphicsMap::TileSpec> iter(curViewSet);
        while (iter.hasNext()) {
            auto tileSpec = iter.next();
            createAscendingTileCache(tileSpec, m_tileTriedToShowdSet);
        }
    }
    QSet<GraphicsMap::TileSpec> realToHideTileSet = needToHideTileSet - m_tileTriedToShowdSet;

    // update the scene tiles
    {
        QSetIterator<GraphicsMap::TileSpec> iter(m_tileTriedToShowdSet);
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

/// \note 该槽函数应该在多线程通过队列调用,以免多线程正在进行上一次资源路径的加载操作
void GraphicsMapThread::requestPath(const QString &path)
{
    if(path == m_path)
        return;
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
    QString fileName = QString("%1/%2/%3/%4")
            .arg(m_path)
            .arg(tileSpec.zoom)
            .arg(tileSpec.x)
            .arg(m_yInverted ? tileCount - tileSpec.y - 1 : tileSpec.y);
    if(QFileInfo::exists(fileName+".jpg"))
        fileName += ".jpg";
    else if(QFileInfo::exists(fileName+".png"))
        fileName += ".png";
    else
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
