#ifndef MAPELLIPSEOPERATOR_H
#define MAPELLIPSEOPERATOR_H

#include "interactivemap.h"
class MapEllipseItem;

class GRAPHICSMAPLIB_EXPORT MapEllipseOperator : public InteractiveMapOperator
{
public:
    MapEllipseOperator();
protected:
    virtual bool mousePressEvent(QMouseEvent *event) override;
    virtual bool mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool mouseMoveEvent(QMouseEvent *event) override;

private:
    MapEllipseItem *m_ellipse;
    QGeoCoordinate m_first;
};

#endif // MAPELLIPSEOPERATOR_H
