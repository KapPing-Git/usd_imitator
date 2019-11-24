#include "achartmousephysicalcoord.h"

AChartMousePhysicalCoord::AChartMousePhysicalCoord()
{
  m_xVisible=true;
  m_yVisible=true;
  m_xValueOrientashion=ValueOrientashion_right;
  m_yYalueOrientashion=ValueOrientashion_top;
  setX_CoordDigitFormat();
  setY_CoordDigitFormat();
}

void AChartMousePhysicalCoord::setXY_visible(bool visible)
{
  m_xVisible = visible;
  m_yVisible = visible;
}

void AChartMousePhysicalCoord::setX_Visible(bool visible)
{
  m_xVisible = visible;
}

void AChartMousePhysicalCoord::setY_Visible(bool visible)
{
  m_yVisible = visible;
}

bool AChartMousePhysicalCoord::xyVisible()
{
  return(m_xVisible && m_yVisible);
}

bool AChartMousePhysicalCoord::xVisible()
{
  return(m_xVisible);
}

bool AChartMousePhysicalCoord::yVisible()
{
  return(m_yVisible);
}

void AChartMousePhysicalCoord::setX_Orientashion(AChartMousePhysicalCoord::ValueOrientashion orientashion)
{
  m_xValueOrientashion=orientashion;
}

void AChartMousePhysicalCoord::setY_Orientashion(AChartMousePhysicalCoord::ValueOrientashion orientashion)
{
  m_yYalueOrientashion=orientashion;
}

AChartMousePhysicalCoord::ValueOrientashion AChartMousePhysicalCoord::xOrientashion()
{
  return(m_xValueOrientashion);
}

AChartMousePhysicalCoord::ValueOrientashion AChartMousePhysicalCoord::yOrientashion()
{
  return(m_yYalueOrientashion);
}

void AChartMousePhysicalCoord::setX_CoordDigitFormat(char f, int precision)
{
  m_xFormat=f;
  m_xPrecision=precision;
}

void AChartMousePhysicalCoord::setY_CoordDigitFormat(char f, int precision)
{
  m_yFormat=f;
  m_yPrecision=precision;
}

int AChartMousePhysicalCoord::xPrecision()
{
  return(m_xPrecision);
}

int AChartMousePhysicalCoord::yPrecision()
{
  return(m_yPrecision);
}

char AChartMousePhysicalCoord::xFormat()
{
  return(m_xFormat);
}

char AChartMousePhysicalCoord::yFormat()
{
  return(m_yFormat);
}
