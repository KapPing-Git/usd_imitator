#include "achartaxispainter.h"

AChartAxisPainter::AChartAxisPainter(AChart *chart) :
    QObject(chart)
{
  m_chart = chart;
  m_xFormat='f';
  m_yFormat='f';
  m_xPrecision=0;
  m_yPrecision=0;
}
QColor AChartAxisPainter::backColor() const
{
  return m_backColor;
}

void AChartAxisPainter::setBackColor(const QColor &backColor)
{
  m_backColor = backColor;
}
char AChartAxisPainter::xFormat() const
{
  return m_xFormat;
}

void AChartAxisPainter::setXFormat(char xFormat)
{
  m_xFormat = xFormat;
}
char AChartAxisPainter::yFormat() const
{
  return m_yFormat;
}

void AChartAxisPainter::setYFormat(char yFormat)
{
  m_yFormat = yFormat;
}
int AChartAxisPainter::xPrecision() const
{
  return m_xPrecision;
}

void AChartAxisPainter::setXPrecision(int xPrecision)
{
  m_xPrecision = xPrecision;
}
int AChartAxisPainter::yPrecision() const
{
  return m_yPrecision;
}

void AChartAxisPainter::setYPrecision(int yPrecision)
{
  m_yPrecision = yPrecision;
}

QSize AChartAxisPainter::gridSize() const
{
  return m_gridSize;
}

void AChartAxisPainter::setGridSize(const QSize &gridSize)
{
  m_gridSize = gridSize;
}





