#include "achartpolyline.h"

AChartPolyline::AChartPolyline():
    QPolygonF()
{
    this->clear();
}

void AChartPolyline::addPoint(qreal x, qreal y)
{
    this->append(QPointF(x, y));
}

bool AChartPolyline::setPoint(int numPoint, qreal x, qreal y)
{
  if (numPoint > size()) return false;
  replace(numPoint,QPointF(x,y));
  return true;
}

QPointF AChartPolyline::point(int index)
{
  return this->at(index);
}

int AChartPolyline::pointsCount()
{
  return this->size();
}

void AChartPolyline::setPointCount(int count)
{
  resize(count);
}

qreal AChartPolyline::pointX(int index)
{
    return point(index).x();
}

qreal AChartPolyline::pointY(int index)
{
    return point(index).y();
}
