#include "achartseriesdecorator.h"

AChartSeriesDecorator::AChartSeriesDecorator(AChart *parent, AChartSeries *series, qreal beginX, qreal endX) :
  AChartDecorator(parent)
{
  m_beginX = beginX;
  m_endX = endX;
  if (parent == NULL) return;
  m_series = series;
  connect(m_series.data(),&AChartSeries::destroyed,this,&AChartSeriesDecorator::deleteI);
}

AChartSeriesDecorator::AChartSeriesDecorator(AChart *parent, int numSeries, qreal beginX, qreal endX) :
  AChartDecorator(parent)
{
  m_beginX = beginX;
  m_endX = endX;
  if (parent == NULL) return;
  m_series = parent->series(numSeries);
  connect(m_series.data(),&AChartSeries::destroyed,this,&AChartSeriesDecorator::deleteI);
}

AChartSeriesDecorator::AChartSeriesDecorator(AChart *parent, QString nameSeries, qreal beginX, qreal endX) :
  AChartDecorator(parent)
{
  m_beginX = beginX;
  m_endX = endX;
  if (parent == NULL) return;
  m_series = parent->series(nameSeries);
  connect(m_series.data(),&AChartSeries::destroyed,this,&AChartSeriesDecorator::deleteI);
}

AChartSeries *AChartSeriesDecorator::series()
{
  return m_series;
}

qreal AChartSeriesDecorator::beginX()
{
  return m_beginX;
}

qreal AChartSeriesDecorator::endX()
{
  return m_endX;
}

QString AChartSeriesDecorator::seriesName()
{
  if (m_series) return m_series->name();
  else return "";
}

void AChartSeriesDecorator::beforeAnySignal()
{
  if (m_series)
    {
      m_savedNameAxis = m_parentChart->axisContainer()->activeAxis()->name();
      m_parentChart->axisContainer()->setActiveAxis(m_parentChart->seriesAxis(m_series.data())->name());
    }
}

void AChartSeriesDecorator::afterAnySignal()
{
  if (m_series)
    m_parentChart->axisContainer()->setActiveAxis(m_savedNameAxis);
}

void AChartSeriesDecorator::deleteI()
{
  delete this;
}
