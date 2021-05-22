#include "interactivemap.h"

InteractiveMap::InteractiveMap(QGraphicsScene *scene) : GraphicsMap(scene)
{

}

void InteractiveMap::wheelEvent(QWheelEvent *e)
{
    bool increase = e->angleDelta().y() > 0;
    if(increase)
        this->setZoomLevel(zoomLevel() + 0.2);
    else
        this->setZoomLevel(zoomLevel() - 0.2);
    e->accept();
}
