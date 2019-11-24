#include "achartaxiscollection.h"

AChartAxisContainer::AChartAxisContainer(QObject *parent) :
  QObject(parent)
{
//  hide();

//  setFont(parent->font());
//  //возможные значения формата -- e,E,f,g,G
//  m_xFormat='f';
//  m_yFormat='f';
//  m_xPrecision=1;
//  m_yPrecision=1;
  m_iterator = new QHashIterator<QString,AChartAxis *>(m_axisCollection);
  m_activeAxis = NULL;
}

AChartAxisContainer::~AChartAxisContainer()
{
  delete(m_iterator);
}

void AChartAxisContainer::addAxis(qreal minX, qreal minY, qreal maxX, qreal maxY, QString name, QColor color)
{
  AChartAxis *axis = new AChartAxis(minX,minY,maxX,maxY,name,color,this);
  m_axisCollection.insert(name,axis);
  if (m_axisCollection.count() == 1) m_activeAxis = axis;
}

void AChartAxisContainer::addAxis(AChartAxis *axis)
{
  m_axisCollection.insert(axis->name(),axis);
  if (m_axisCollection.count() == 1) m_activeAxis = axis;
}

AChartAxis *AChartAxisContainer::axis(QString nameAxis)
{
  if (m_axisCollection.contains(nameAxis))
    return m_axisCollection.value(nameAxis);
  else
    return activeAxis();
}

AChartAxis *AChartAxisContainer::axis(int numAxis)
{
  return m_axisCollection.values().at(numAxis);
}

void AChartAxisContainer::setActiveAxis(QString nameAxis)
{
  if (m_axisCollection.contains(nameAxis)) m_activeAxis = m_axisCollection[nameAxis];
}

void AChartAxisContainer::setActiveAxis(int numAxis)
{
  if ( (numAxis >=0 ) && (numAxis < m_axisCollection.count()) )
    m_activeAxis = m_axisCollection.values().at(numAxis);
}

AChartAxis *AChartAxisContainer::activeAxis()
{
  return m_activeAxis;
}

int AChartAxisContainer::activeAxisNum()
{
  return m_axisCollection.values().indexOf(m_activeAxis);
}

bool AChartAxisContainer::contains(QString nameAxis)
{
  return m_axisCollection.contains(nameAxis);
}

bool AChartAxisContainer::contains(int numAxis)
{
  if ( (m_axisCollection.count() != 0) and (numAxis >= 0 ) and (numAxis < m_axisCollection.count()) ) return true;
  else return false;
}

void AChartAxisContainer::deleteAxis(QString nameAxis)
{
  m_axisCollection.remove(nameAxis);
}

void AChartAxisContainer::toFront()
{
  m_iterator->toFront();
}

void AChartAxisContainer::toBack()
{
  m_iterator->toBack();
}

AChartAxis *AChartAxisContainer::next()
{
  return m_iterator->next().value();
}

AChartAxis *AChartAxisContainer::previos()
{
  return m_iterator->previous().value();
}

bool AChartAxisContainer::hasNext()
{
  return m_iterator->hasNext();
}

bool AChartAxisContainer::hasPrevios()
{
  return m_iterator->hasPrevious();
}

int AChartAxisContainer::count()
{
  return m_axisCollection.count();
}

qreal AChartAxisContainer::minX()
{
  if(count() == 0) return 0;
  int result = axis(0)->seriesCollection()->minX();
  for (int i = 0;i < count();i++)
    {
      if (axis(i)->seriesCollection()->minX() < result) result = axis(i)->seriesCollection()->minX();
    }
  return result;
}

qreal AChartAxisContainer::maxX()
{
  if(count() == 0) return 0;
  int result = axis(0)->seriesCollection()->maxX();
  for (int i = 0;i < count();i++)
    {
      if (axis(i)->seriesCollection()->maxX() > result) result = axis(i)->seriesCollection()->maxX();
    }
  return result;
}

qreal AChartAxisContainer::minY()
{
  if(count() == 0) return 0;
  int result = axis(0)->seriesCollection()->minY();
  for (int i = 0;i < count();i++)
    {
      if (axis(i)->seriesCollection()->minY() < result) result = axis(i)->seriesCollection()->minY();
    }
  return result;
}

qreal AChartAxisContainer::maxY()
{
  if(count() == 0) return 0;
  int result = axis(0)->seriesCollection()->maxY();
  for (int i = 0;i < count();i++)
    {
      if (axis(i)->seriesCollection()->maxY() > result) result = axis(i)->seriesCollection()->maxY();
    }
  return result;
}

void AChartAxisContainer::onSeriesChanged()
{

}

//void AChartAxisContainer::setX_Format(char format)
//{
//  m_xFormat = format;
//}

//int AChartAxisContainer::axisWidth()
//{
//  return(m_decoratedWidget->fontMetrics().width('9')*(yPrecision()+2));
//}

//int AChartAxisContainer::axisHeigth()
//{
//  return(m_decoratedWidget->fontMetrics().height()*1.5);
//}

//char AChartAxisContainer::xFormat()
//{
//  return(m_xFormat);
//}

//void AChartAxisContainer::setY_Format(char format)
//{
//  m_yFormat = format;
//}

//char AChartAxisContainer::yFormat()
//{
//  return(m_yFormat);
//}

//void AChartAxisContainer::setXPrecision(int precision)
//{
//  m_xPrecision = precision;
//}

//int AChartAxisContainer::xPrecision()
//{
//  return(m_xPrecision);
//}

//void AChartAxisContainer::setYPrecision(int precision)
//{
//  m_yPrecision = precision;
//}

//int AChartAxisContainer::yPrecision()
//{
//  return(m_yPrecision);
//}
