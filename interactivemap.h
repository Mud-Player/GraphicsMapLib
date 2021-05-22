#ifndef INTERACTIVEMAP_H
#define INTERACTIVEMAP_H

#include "graphicsmap.h"

/*!
 * \brief 可交互地图
 * \details 该地图可实现鼠标缩放地图，配合Operator操作器实现地图对象的创建
 */
class GRAPHICSMAPLIB_EXPORT InteractiveMap : public GraphicsMap
{
    Q_OBJECT
public:
    InteractiveMap(QGraphicsScene *scene);

protected:
    virtual void wheelEvent(QWheelEvent *e) override;

};

#endif // INTERACTIVEMAP_H
