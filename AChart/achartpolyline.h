#ifndef ACHARTPOLYGON_H
#define ACHARTPOLYGON_H

#include <QPolygonF>

class AChartPolyline : public QPolygonF
{
public:
    AChartPolyline();

    void addPoint(qreal x, qreal y);
    bool setPoint(int numPoint,qreal x,qreal y);

    QPointF point(int index);
    int pointsCount();
    void setPointCount(int count);

    qreal pointX(int index);
    qreal pointY(int index);

};

#endif // ACHARTPOLYGON_H
