#include "achartrectselector.h"
#include <QLinearGradient>
#include <QMenu>

AChartRectSelector::AChartRectSelector(AChart *parent) :
  AChartDecorator(parent)
{
  m_areaRect = QRectF(0,0,0,0);
  m_stage = stage_firtsPointSetting;
  m_borderWidth = 20;
  m_borderVisible = false;
  m_areaColor = Qt::green;
  m_areaPen = QPen(Qt::black);
  m_borderPen = QPen(Qt::DashLine);
  m_transparency = 70;
  m_menuReady = false;
  delAction = menu.addAction(tr("Удалить"));
  connect(delAction,&QAction::triggered,this,&QObject::deleteLater);
}

void AChartRectSelector::toStartStage()
{
  m_areaRect = QRectF(0,0,0,0);
  m_stage = stage_firtsPointSetting;
}

QString AChartRectSelector::typeName()
{
  return "rectSelector";
}

QByteArray AChartRectSelector::data()
{
  QByteArray byteArray;
  QDataStream out(&byteArray,QIODevice::WriteOnly);
  out << CURRENT_VERSION << m_areaRect << m_borderWidth;
  return byteArray;
}

void AChartRectSelector::setData(QByteArray *byteArray)
{
  quint16 version;
  QDataStream in(byteArray,QIODevice::ReadOnly);
  in >> version;
  if (version == CURRENT_VERSION)
    {
      in >> m_areaRect >> m_borderWidth;
      m_borderVisible = false;
      m_stage = stage_complete;
    }
  else//здесь должна быть функция преобразования версии(когда нужно будет изменить сохраняемые данные)
    {

    }
}

AChartDecorator *AChartRectSelector::cloneMyType(AChart *chart)
{
  return new AChartRectSelector(chart);
}

void AChartRectSelector::mousePress(QMouseEvent *event)
{
  QPointF realPoint = m_parentChart->screenToPhysicalCoords(event->pos());
  if (event->button() == Qt::LeftButton)
    {
      if (m_stage == stage_firtsPointSetting)
        {
          m_firstPoint = realPoint;
          m_stage = stage_twoPointSetting;
        }
      if (m_stage == stage_complete)
        {
          m_firstPoint = realPoint;
          m_deltaToTopLeft = m_areaRect.topLeft() - m_firstPoint;
          m_deltaRight = m_areaRect.right() - m_firstPoint.x();
          m_deltaTop = m_areaRect.bottom() - m_firstPoint.y();
          m_deltaBottom = m_areaRect.top() - m_firstPoint.y();
          if (m_areaRect.contains(realPoint)) m_stage = stage_moving;
          if (leftRect().contains(event->pos())) m_stage = stage_leftMoving;
          if (rightRect().contains(event->pos())) m_stage = stage_rightMoving;
          if (topRect().contains(event->pos())) m_stage = stage_topMoving;
          if (bottomRect().contains(event->pos())) m_stage = stage_bottomMoving;
        }
    }
  if (event->button() == Qt::RightButton)
    {
      m_forMenuPointBegin = event->pos();
      m_menuReady = true;
    }
}

void AChartRectSelector::mouseRelease(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton)
    {
      if (m_stage != stage_complete) emit objectBuildered();
      m_stage = stage_complete;
    }
  if (event->button() == Qt::RightButton)
    {
      m_forMenuPointEnd = event->pos();
      if (m_menuReady)
        {
          if(QLineF(m_forMenuPointBegin,m_forMenuPointEnd).length() < 4)
          {
            menu.move(event->globalPos());
            menu.show();
          }
        }
      m_menuReady = true;
    }
}

void AChartRectSelector::mouseMove(QMouseEvent *event)
{
  QPointF realPoint = m_parentChart->screenToPhysicalCoords(event->pos());
  if (m_stage == stage_firtsPointSetting)
    {
      m_areaRect.setTopLeft(realPoint);
      m_areaRect.setBottomRight(realPoint);
      emit objectChanged();
    }
  if (m_stage == stage_twoPointSetting)
    {
      m_secondPoint = realPoint;
      m_areaRect.setTopLeft(topLeft());
      m_areaRect.setBottomRight(bottomRight());
      emit objectChanged();
    }
  if (m_stage == stage_moving)
    {
      m_secondPoint = realPoint;
      m_areaRect.moveTopLeft(m_secondPoint + m_deltaToTopLeft);
      emit objectChanged();
    }
  if (m_stage == stage_leftMoving)
    {
      m_secondPoint = realPoint;
      m_areaRect.setLeft(m_secondPoint.x() + m_deltaToTopLeft.x());
      emit objectChanged();
    }
  if (m_stage == stage_rightMoving)
    {
      m_secondPoint = realPoint;
      m_areaRect.setRight(m_secondPoint.x() + m_deltaRight);
      emit objectChanged();
    }
  if (m_stage == stage_topMoving)
    {
      m_secondPoint = realPoint;
      m_areaRect.setBottom(m_secondPoint.y() + m_deltaTop);
      emit objectChanged();
    }
  if (m_stage == stage_bottomMoving)
    {
      m_secondPoint = realPoint;
      m_areaRect.setTop(m_secondPoint.y() + m_deltaBottom);
      emit objectChanged();
    }
  if (m_stage == stage_complete)
    {

    }
  m_areaRect = m_areaRect.normalized();
}

void AChartRectSelector::wheel(QWheelEvent *event)
{
  event = event;
}

bool AChartRectSelector::isMouseMoveTransperency()
{
  return true;
}

bool AChartRectSelector::isMousePressTransperency()
{
  return true;
}

bool AChartRectSelector::isMouseReleaseTransperency()
{
  return true;
}

bool AChartRectSelector::isWheelTransperency()
{
  return true;
}

void AChartRectSelector::draw(QPaintDevice *device, QRect chartRect)
{
  QPainter painter(device);
  painter.setClipRect(chartRect,Qt::ReplaceClip);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.setPen(m_areaPen);
  QRect mainRect = screenRect();
  painter.drawRect(mainRect);
  QPoint leftCenter = QPoint(mainRect.left(),mainRect.center().y());
  QPoint rightCenter = QPoint(mainRect.right(),mainRect.center().y());

  int r = m_areaColor.red();
  int g = m_areaColor.green();
  int b = m_areaColor.blue();
  float z = 0.5;
  int alfa = m_transparency;
  QColor startColor = QColor(r * z,g * z,b * z,alfa);
  QColor middleColor = QColor(r,g,b,alfa);
  QColor endColor = QColor(r * z,g * z,b * z,alfa);
  QLinearGradient gradient(leftCenter,rightCenter);
  gradient.setColorAt(0,startColor);
  gradient.setColorAt(0.5,middleColor);
  gradient.setColorAt(1,endColor);

  QBrush brush = QBrush(gradient);
  //рисуем
  painter.fillRect(mainRect,brush);
  if (m_borderVisible)
    {
      painter.setPen(m_borderPen);
      leftCenter = QPoint(leftRect().left(),leftRect().center().y());
      rightCenter = QPoint(leftRect().right(),leftRect().center().y());
      gradient.setStart(leftCenter);
      gradient.setFinalStop(rightCenter);
      brush = QBrush(gradient);
      painter.fillRect(leftRect(),brush);
      painter.drawRect(leftRect());

      leftCenter = QPoint(rightRect().left(),rightRect().center().y());
      rightCenter = QPoint(rightRect().right(),rightRect().center().y());
      gradient.setStart(leftCenter);
      gradient.setFinalStop(rightCenter);
      brush = QBrush(gradient);
      painter.fillRect(rightRect(),brush);
      painter.drawRect(rightRect());

      QPoint topCenter = QPoint(topRect().center().x(),topRect().top());
      QPoint bottomCenter = QPoint(topRect().center().x(),topRect().bottom());
      gradient.setStart(topCenter);
      gradient.setFinalStop(bottomCenter);
      brush = QBrush(gradient);
      painter.fillRect(topRect(),brush);
      painter.drawRect(topRect());

      topCenter = QPoint(bottomRect().center().x(),bottomRect().top());
      bottomCenter = QPoint(bottomRect().center().x(),bottomRect().bottom());
      gradient.setStart(topCenter);
      gradient.setFinalStop(bottomCenter);
      brush = QBrush(gradient);
      painter.fillRect(bottomRect(),brush);
      painter.drawRect(bottomRect());
    }

}

bool AChartRectSelector::containsScreenPoint(QPoint point)
{
  if (m_stage != stage_complete)
    {
      m_containScreenPoint = true;
      m_borderVisible = m_containScreenPoint;
      return m_containScreenPoint;
    }
  //добавляем ширину бордюра
  QRect fullRect = screenRect().adjusted(-m_borderWidth,m_borderWidth,m_borderWidth,-m_borderWidth);
  m_containScreenPoint = fullRect.contains(point);
  m_borderVisible = m_containScreenPoint;
  return fullRect.contains(point);
}

bool AChartRectSelector::isCanDraw()
{
  if (m_stage != stage_complete) return true;
  //добавляем ширину бордюра
  QRect fullRect = screenRect().adjusted(-m_borderWidth,m_borderWidth,m_borderWidth,-m_borderWidth);
  //возвращаем видна ли наша область на экране
  return fullRect.intersects(m_parentChart->geometry());
}
QPen AChartRectSelector::borderPen() const
{
  return m_borderPen;
}

void AChartRectSelector::setBorderPen(const QPen &borderPen)
{
  m_borderPen = borderPen;
}

QPen AChartRectSelector::areaPen() const
{
  return m_areaPen;
}

void AChartRectSelector::setAreaPen(const QPen &areaPen)
{
  m_areaPen = areaPen;
}

QColor AChartRectSelector::areaColor() const
{
  return m_areaColor;
}

void AChartRectSelector::setAreaColor(const QColor &areaColor)
{
  m_areaColor = areaColor;
}


QPointF AChartRectSelector::topLeft()
{
  qreal x = qMin(m_firstPoint.x(),m_secondPoint.x());
  qreal y = qMax(m_firstPoint.y(),m_secondPoint.y());
  return QPointF(x,y);
}

QPointF AChartRectSelector::bottomRight()
{
  qreal x = qMax(m_firstPoint.x(),m_secondPoint.x());
  qreal y = qMin(m_firstPoint.y(),m_secondPoint.y());
  return QPointF(x,y);
}

QRect AChartRectSelector::screenRect()
{
  QPoint topLeft = m_parentChart->physicalToScreenCoords(m_areaRect.topLeft());
  QPoint bottomRight = m_parentChart->physicalToScreenCoords(m_areaRect.bottomRight());
  return QRect(topLeft,bottomRight);
}

QRect AChartRectSelector::leftRect()
{
  return QRect(screenRect().left() - m_borderWidth, screenRect().top(), m_borderWidth,screenRect().height());
}

QRect AChartRectSelector::rightRect()
{
  return QRect(screenRect().right(), screenRect().top(), m_borderWidth,screenRect().height());
}

QRect AChartRectSelector::topRect()
{
  return QRect(screenRect().left(), screenRect().bottom() - m_borderWidth, screenRect().width(), m_borderWidth);
}

QRect AChartRectSelector::bottomRect()
{
  return QRect(screenRect().left(), screenRect().top(), screenRect().width(),m_borderWidth);
}
int AChartRectSelector::borderWidth() const
{
  return m_borderWidth;
}

void AChartRectSelector::setBorderWidth(int borderWidth)
{
  m_borderWidth = borderWidth;
}


QRectF AChartRectSelector::areaRect() const
{
  return m_areaRect;
}

void AChartRectSelector::setAreaRect(const QRectF &areaRect)
{
  m_areaRect = areaRect;
}

