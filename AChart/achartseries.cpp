#include "achartseries.h"
#include "math.h"
#include <QPen>
#include <QMessageBox>
#include <QTextCodec>

AChartSeries::AChartSeries(QObject *parent, QString name) : QObject(parent)
{
  m_name = name;
  m_realPolyline = new AChartPolyline();
  m_optimalPolyline = new AChartPolyline();
  m_useOptimal = false;
  m_transaction = false;
  m_Pen = new QPen(Qt::black);
  m_normalTrend = false; //true
  m_maxX = 0;
  m_minX = 0;
  m_maxY = 0;
  m_minY = 0;
  m_screenPerPhysX = 0.1;
  m_currScreenX = 0;
  m_startX = 0;
  m_visible = true;
  m_cashedScreenWidth = 100000;
}

AChartSeries::~AChartSeries()
{
  emit destroyed(this);
  delete(m_Pen);
  m_realPolyline->clear();
  delete(m_realPolyline);
  m_optimalPolyline->clear();
  delete(m_optimalPolyline);
}


AChartPolyline* AChartSeries::polyline() const
{
  return m_realPolyline;
}

AChartPolyline *AChartSeries::getOptimalPolyline(int screenWidth, QRectF physicalView)
{
  // надо проверить изминилась ли ширина экрана
  if (m_realPolyline->count() < 1)
    {
      return m_realPolyline;
      m_useOptimal = false;
    }

  if ((m_cashedScreenWidth != screenWidth) || (! equalASPercent(physicalView.width(), m_cashedPhysicalView.width(),5)))
    {
      if (m_realPolyline->count() > 100)
        {
          updateOptimalPolylineInfo(screenWidth,physicalView);
          m_useOptimal = true;
        }
    }
  if (m_realPolyline->count()*2 <= m_optimalPolyline->count())
    {
      m_useOptimal = false;
    }
  if (m_useOptimal)
    return m_optimalPolyline;
  else
    return m_realPolyline;
  //    return m_realPolyline;
}

void AChartSeries::addPoint(qreal x, qreal y)
{
  //m_cashedScreenWidth = 0;
  m_realPolyline->addPoint(x,y);


  // добавляем точку в оптимизированиый график
  if (!m_transaction)
    {
      if ( (x - m_startX) * (m_screenPerPhysX) < m_currScreenX ) // если точка в следующем пикселе
        {
          if (m_lastPointMin.x()> m_lastPointMax.x())
            {
              m_optimalPolyline->append(m_lastPointMax);
              m_optimalPolyline->append(m_lastPointMin);
            }
          else
            {
              m_optimalPolyline->append(m_lastPointMin);
              m_optimalPolyline->append(m_lastPointMax);
            }
          m_currScreenX++;
          m_lastPointMax = QPointF(x,y);
          m_lastPointMin = QPointF(x,y);
        }
      else
        {
          if (m_lastPointMax.y()<y) m_lastPointMax = QPointF(x,y);
          if (m_lastPointMin.y()>y) m_lastPointMin = QPointF(x,y);
        }
    }


  //считаем минимумы и максимумы
  if (pointsCount()==1)
    {
      m_minX=x;
      m_minY=y;
      m_maxX=x;
      m_maxY=y;
    }
  else
    {
      if (x<m_minX) m_minX=x;
      if (x>m_maxX) m_maxX=x;
      //если появилась точка меньщая по Х предыдущей, то такой график будем считать ненормальным
      else m_normalTrend=false;
      //---
      if (y<m_minY) m_minY=y;
      if (y>m_maxY) m_maxY=y;
    }
  //-----
  if (!m_transaction) emit changed();
}

void AChartSeries::addPoint(QDateTime dt, qreal value)
{
  addPoint((qreal)dt.toMSecsSinceEpoch(),value);
}

void AChartSeries::setPoint(int numPoint, qreal x, qreal y)
{
  m_realPolyline->setPoint(numPoint,x,y);
  //обновляем минимумы и максимумы
  if (x < m_minX) m_minX = x;
  if (x > m_maxX) m_maxX = x;
  if (y < m_minY) m_minY = y;
  if (y > m_maxY) m_maxY = y;

  //проверяем не стал ли график ненормальным
  if ( (numPoint > 0) && (numPoint < m_realPolyline->count() - 1) )
    {
      if ( (x < m_realPolyline->at(numPoint - 1).x()) || (x > m_realPolyline->at(numPoint - 1).x()) )
        m_normalTrend = false;
    }
  else
    {
      if ( (numPoint == 0) && (m_realPolyline->count() > 0) )
        {
          if (x > m_realPolyline->at(numPoint + 1).x())
            m_normalTrend = false;
        }
      if (numPoint == m_realPolyline->count() - 1)
        {
          if (x < m_realPolyline->at(numPoint - 1).x())
            m_normalTrend = false;
        }
    }

}

void AChartSeries::setPointCount(int count)
{
  m_realPolyline->setPointCount(count);
}

void AChartSeries::updateOptimalPolylineInfo(int screenWidth,QRectF physicalView)
{
  m_cashedScreenWidth = screenWidth;
  m_cashedPhysicalView = physicalView;
  // отчищаем оптимальный полилайн
  m_optimalPolyline->clear();
  // пересчет точек для конкретного размера
  qreal screenPerPhysX = screenWidth / physicalView.width();

  int currScreenX = 0;
  QPointF pointMax ;
  QPointF pointMin ;

  //  if (m_realPolyline->count() > 100)
  //    {
  float startX = m_realPolyline->at(0).x();
  for(int i=0;i<m_realPolyline->count();i++)
    {

      pointMax = m_realPolyline->at(i);
      pointMin = m_realPolyline->at(i);

      // пока точка входит в пиксель
      while (i < (m_realPolyline->count() - 1) && ((m_realPolyline->at(i).x()-startX)*(screenPerPhysX) < currScreenX ))
        {
          i++;
          if (pointMax.y()<m_realPolyline->at(i).y()) pointMax = m_realPolyline->at(i);
          if (pointMin.y()>m_realPolyline->at(i).y()) pointMin = m_realPolyline->at(i);

        }
      if (pointMin.x()> pointMax.x())
        {
          m_optimalPolyline->append(pointMax);
          m_optimalPolyline->append(pointMin);
        }
      else
        {
          m_optimalPolyline->append(pointMin);
          m_optimalPolyline->append(pointMax);
        }
      currScreenX++;
      //        }
      // для кеша расчетов добавления точки
      //
      m_lastPointMax = pointMax;
      m_lastPointMin = pointMin;
      m_screenPerPhysX = screenPerPhysX;
      m_currScreenX = currScreenX;
      m_startX = startX;


    }
}

QPointF AChartSeries::point(int index)
{
  return(m_realPolyline->at(index));
}

QPointF AChartSeries::lastPoint()
{
  if (m_realPolyline->count() == 0) return(QPointF(0,0));
  else return(m_realPolyline->at(m_realPolyline->count()-1));
}

void AChartSeries::setData(QByteArray *data)
{
  clear();
  QDataStream out(*data);
  out.setVersion(QDataStream::Qt_4_8);
  quint16 version;
  out >> version;
  if (version != CURRENT_VERSION)
    {//при изменении данных предусмотреть метод преобразования
      QMessageBox::warning(NULL, tr("USD60Explorer"),
                           tr("Ошибка чтения данных графика"),
                           QMessageBox::Ok);
      return;
    }

  out >> m_maxX >> m_maxY >> m_minX >> m_minY >> m_name >> m_normalTrend >> *m_Pen >> m_visible;
  startTransaction();
  while (!out.atEnd())
    {
      QPointF point;
      out >> point;
      addPoint(point.x(),point.y());
    }
  stopTransaction();
}

void AChartSeries::clear()
{
  m_realPolyline->clear();
}

void AChartSeries::startTransaction()
{
  m_transaction=true;

}

void AChartSeries::stopTransaction()
{
  m_transaction=false;
  updateOptimalPolylineInfo(m_cashedScreenWidth,m_cashedPhysicalView);
  emit changed();

}

void AChartSeries::setPen(QPen *pen)
{
  m_Pen=pen;
}

QPen * AChartSeries::pen()
{
  return(m_Pen);
}

qreal AChartSeries::minX()
{
  return(m_minX);
}

qreal AChartSeries::minY()
{
  return(m_minY);
}

qreal AChartSeries::maxX()
{
  return(m_maxX);
}

qreal AChartSeries::maxY()
{
  return(m_maxY);
}

qreal AChartSeries::averageDeltaX()
{
  return((maxX()-minX())/pointsCount());
}

bool AChartSeries::isNormalTrend()
{
  return(m_normalTrend);
}

int AChartSeries::indexOf(qreal x)
{
  if (m_realPolyline->isEmpty()) return(0);

  if (isNormalTrend())
    {
      qint64 startPoint = (x - m_realPolyline->at(0).x()) / averageDeltaX();
      qint64 currPoint = startPoint;
      //ищем точку равную или ближайщую к заданной по X
      while(1)
        {
          if ((currPoint <= 1))
            {
              return 0;
            }
          if ((currPoint >= pointsCount() - 1))
            {
              return pointsCount() - 1;
            }

          qreal currX = m_realPolyline->at(currPoint).x();
          qreal prevX = m_realPolyline->at(currPoint-1).x();
          qreal nextX = m_realPolyline->at(currPoint+1).x();
          //если искомая точка равна текущей, или находится между соседними
          if ((x == currX) || ((x>prevX) && (x<nextX)))
            {
              return(currPoint);
            }
          if (x <= prevX) currPoint--;
          if (x >= nextX) currPoint++;
        }
    }
  else
    {
      int indexFindedPoint = 0;
      qreal minDelta = qAbs(x - m_realPolyline->point(0).x());
      for (int i = 1;i < m_realPolyline->count();i++)
        {
          qreal delta = qAbs(x - m_realPolyline->point(i).x());
          if (delta < minDelta)
            {
              indexFindedPoint = i;
              minDelta = delta;
            }
        }
      return(indexFindedPoint);
    }


}


QString AChartSeries::name()
{
  return m_name;
}

QByteArray *AChartSeries::data()
{
  QByteArray *data=new QByteArray();
  QDataStream in(data,QIODevice::WriteOnly);
  in.setVersion(QDataStream::Qt_4_8);
  in << CURRENT_VERSION;
  in << m_maxX << m_maxY << m_minX << m_minY << m_name << m_normalTrend << *m_Pen << m_visible;
  for (int i=0;i<m_realPolyline->count();i++)
    {
      in << m_realPolyline->point(i);
    }
  return(data);
}

int AChartSeries::pointsCount()
{
  return this->polyline()->size();
}

bool AChartSeries::visible() const
{
  return m_visible;
}

void AChartSeries::setVisible(bool visible)
{
  m_visible = visible;
  emit changed();
}

qreal AChartSeries::intervalMin(qreal beginX, qreal endX)
{
  if ( (m_realPolyline->count() == 0) || (beginX > m_maxX) || (endX < m_minX) ) return 0;
  if (m_normalTrend)
    {
      int beginI = indexOf(beginX);
      int endI = indexOf(endX);
      qreal result = m_realPolyline->point(beginI).y();
      for (int i = beginI; i<endI;i++)
        {
          if(m_realPolyline->point(i).y() < result) result = m_realPolyline->point(i).y();
        }
      return(result);
    }
  else
    {
      qreal result = m_realPolyline->point(0).y();
      for (int i = 0;i < m_realPolyline->count();i++)
        {
          if ( (m_realPolyline->point(i).x() > beginX) and (m_realPolyline->point(i).x() < endX) )
            {
              if(m_realPolyline->point(i).y() < result) result = m_realPolyline->point(i).y();
            }
        }
      return(result);
    }
}

qreal AChartSeries::intervalMax(qreal beginX, qreal endX)
{
  if ( (m_realPolyline->count() == 0) || (beginX > m_maxX) || (endX < m_minX) ) return 0;
  if (m_normalTrend)
    {
      int beginI = indexOf(beginX);
      int endI = indexOf(endX);
      qreal result = m_realPolyline->point(beginI).y();
      for (int i = beginI; i<endI;i++)
        {
          if(m_realPolyline->point(i).y() > result) result = m_realPolyline->point(i).y();
        }
      return(result);
    }
  else
    {
      qreal result = m_realPolyline->point(0).y();
      for (int i = 0;i < m_realPolyline->count();i++)
        {
          if ( (m_realPolyline->point(i).x() > beginX) and (m_realPolyline->point(i).x() < endX) )
            {
              if(m_realPolyline->point(i).y() > result) result = m_realPolyline->point(i).y();
            }
        }
      return(result);
    }
}

qreal AChartSeries::intervalAverage(qreal beginX, qreal endX)
{
  if ( (m_realPolyline->count() == 0) || (beginX > m_maxX) || (endX < m_minX) ) return 0;
  if (m_normalTrend)
    {
      int beginI = indexOf(beginX);
      int endI = indexOf(endX);
      qreal result = 0;
      for (int i = beginI; i<endI;i++)
        {
          result += m_realPolyline->point(i).y();
        }
      result = result / (endI - beginI);
      return(result);
    }
  else
    {
      qreal result = 0;
      int intervalPointCount = 0;
      for (int i = 0;i < m_realPolyline->count();i++)
        {
          if ( (m_realPolyline->point(i).x() > beginX) and (m_realPolyline->point(i).x() < endX) )
            {
              result += m_realPolyline->point(i).y();
              intervalPointCount++;
            }
        }
      result = result / intervalPointCount;
      return(result);
    }
}

void AChartSeries::upgrade()
{
  onUpgrade();

}

bool AChartSeries::equalASPercent(qreal firstVal, qreal secondVal, qreal percent)
{
  if ( (qAbs(firstVal - secondVal) / qAbs(qMax(firstVal,secondVal))) > (percent / 100) ) return false;
  else return true;
}

void AChartSeries::onUpgrade()
{
  // ни чего не делаем
}
