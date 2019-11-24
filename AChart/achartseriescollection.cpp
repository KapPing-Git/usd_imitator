#include "achartseriescollection.h"

AChartSeriesCollection::AChartSeriesCollection() :
    QHash<QString, AChartSeries *>()
{

}


void AChartSeriesCollection::AddSeries(AChartSeries *series)
{
    this->insert(series->name(), series);
}

AChartSeries* AChartSeriesCollection::series(int index)
{
    return this->values().at(index);
}

AChartSeries* AChartSeriesCollection::series(QString name)
{
    return this->value(name);
}

int AChartSeriesCollection::seriesCount()
{
    return this->size();
}


//------------------------------------------------------
AChartPolyline* AChartSeriesCollection::seriesPolyline(int seriesIndex)
{
  return this->series(seriesIndex)->polyline();
}

//если хотябы один из трендов непустой то возвращаем false
bool AChartSeriesCollection::isAllSeriesEmpty()
{
  if (isEmpty()) return(true);
  bool result = true;
  for (int i = 0;i < seriesCount();i++)
    {
      if (series(i)->pointsCount() > 0) result = false;
    }
  return result;
}

qreal AChartSeriesCollection::minX()
{
  if (seriesCount()==0) return(0);
  qreal minX=series(0)->minX();
  for (int i=0;i<seriesCount()-1;i++)
    {
      if (series(i)->minX()<minX) minX=series(i)->minX();
    }
  return(minX);
}

qreal AChartSeriesCollection::minY()
{
  if (seriesCount()==0) return(0);
  qreal minY=series(0)->minY();
  for (int i=0;i<seriesCount()-1;i++)
    {
    if (series(i)->minY()<minY) minY=series(i)->minY();
    }
  return(minY);
}

qreal AChartSeriesCollection::maxX()
{
  if (seriesCount()==0) return(0);
  qreal maxX=series(0)->maxX();
  for (int i=0;i<seriesCount()-1;i++)
    {
      if (series(i)->maxX()>maxX) maxX=series(i)->maxX();
    }
  return(maxX);
}

qreal AChartSeriesCollection::maxY()
{
  if (seriesCount()==0) return(0);
  qreal maxY=series(0)->maxY();
  for (int i=0;i<seriesCount()-1;i++)
    {
      if (series(i)->maxY()>maxY) maxY=series(i)->maxY();
    }
  return(maxY);
}


QByteArray AChartSeriesCollection::data()
{
  QByteArray data;
  QDataStream in(&data,QIODevice::WriteOnly);
  in << VERSION;
//  for (int i=0;i<m_;i++)
//    {
//      in << m_realPolyline->point(i);
//    }
  return(data);
}
