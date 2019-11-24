#include "achartdecorator.h"

AChartDecorator::AChartDecorator(AChart *parent) :
  QObject(parent)
{
  m_parentChart = parent;
  m_visible = true;
  if (parent == NULL) return;
  m_parentChart->addChartDecorator(this);

}

AChartDecorator::~AChartDecorator()
{
  emit destroyed(this);
}

void AChartDecorator::drawEvent(QPaintDevice *device, QRect chartRect)
{
  if (!visible()) return;
  beforeAnySignal();
  if (isCanDraw())
    {   
      draw(device, chartRect);
    }
  afterAnySignal();
}

bool AChartDecorator::mousePressEvent(QMouseEvent *event)
{
  if (!visible()) return false;
  beforeAnySignal();
  bool result = false;
  if (containsScreenPoint(event->pos()))
    {     
      mousePress(event);    
      if (isMousePressTransperency()) result = false;
      else result = true;
    }
  else result = false;
  afterAnySignal();
  return result;
}

bool AChartDecorator::mouseReleaseEvent(QMouseEvent *event)
{
  if (!visible()) return false;
  beforeAnySignal();
  bool result = false;
  if (containsScreenPoint(event->pos()))
    {    
      mouseRelease(event);      
      if (isMouseReleaseTransperency()) result = false;
      else result = true;
    }
  else result = false;
  afterAnySignal();
  return result;
}

bool AChartDecorator::mouseMoveEvent(QMouseEvent *event)
{
  if (!visible()) return false;
  beforeAnySignal();
  bool result = false;
  if (containsScreenPoint(event->pos()))
    {      
      mouseMove(event);      
      if (isMouseMoveTransperency()) result = false;
      else result = true;
    }
  else result = false;
  afterAnySignal();
  return result;
}

bool AChartDecorator::wheelEvent(QWheelEvent *event)
{
  if (!visible()) return false;
  beforeAnySignal();
  bool result = false;
  if (containsScreenPoint(event->pos()))
    {      
      wheel(event);      
      if (isWheelTransperency()) result = false;
      else result = true;
    }
  else result = false;
  afterAnySignal();
  return result;
}

void operator <<(QDataStream &stream, AChartDecorator &decorator)
{
  stream << decorator.data();
}

void operator >>(QDataStream &stream, AChartDecorator &decorator)
{
  QByteArray data;
  stream >> data;
  decorator.setData(&data);
}

bool AChartDecorator::isMousePressTransperency()
{
  return false;
}

bool AChartDecorator::isMouseReleaseTransperency()
{
  return false;
}

bool AChartDecorator::isMouseMoveTransperency()
{
  return false;
}

bool AChartDecorator::isWheelTransperency()
{
  return false;
}


bool AChartDecorator::visible() const
{
  return m_visible;
}

void AChartDecorator::setVisible(bool visible)
{
  m_visible = visible;
}

void AChartDecorator::beforeAnySignal()
{

}

void AChartDecorator::afterAnySignal()
{

}
