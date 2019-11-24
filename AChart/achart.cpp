#include "achart.h"
#include <math.h>
#include <QTime>
#include <QMessageBox>
#include <math.h>
#include <QDebug>
#include <../AChart/AChartAxisPainter/achartaxispaintersimple.h>

AChart::AChart(QWidget *parent) :
  QWidget(parent)
{
  this->setMouseTracking(true);

  m_mouseX = 0;
  m_mouseY = 0;
  m_mouseRightPressed = false;
  m_mouseLeftPressed = false;

  m_physUnitPerPixelX = 1;
  m_physUnitPerPixelY = 1;

  setChartMode( AChart::ChartMode_View );

  m_axisContainer = new AChartAxisContainer(this);
  m_axisContainer->addAxis(new AChartAxis(0,0,0,0,STD_AXIS,Qt::black,this));

  m_axisPainter = new AChartAxisPainterSimple(this);
  setPhysicalView(0,0,0,0);

  m_chartBackColor = palette().color(QPalette::Normal,QPalette::Background);

  this->setWheelZoomPercent(10, 10);

  m_repaintTime=new QTime();
  m_repaintTime->start();

  setMinimumWidth(100);
  setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
  m_viewMode = ViewMode_allOnScreen;
  userActionChangeViewMode(true);

  createFormComponent();
  setSliderVisibleMode(sliderVisibleMode_isNeedVisible);
  m_decoratorsVisible = true;
  m_chartRect = geometry()/*QRect(0,0,0,0)*/;
  m_mouseChangeAllAxisByX = false;
  m_mouseChangeAllAxisByY = false;

  //m_render = QPainter::Antialiasing;
  m_render = QPainter::HighQualityAntialiasing;

  m_resizeOnScrool = true;
  m_mouseMoveAxis = true;

  setViewModeMargins(0,0);
}

AChart::~AChart()
{
  delete(m_repaintTime);
  removeChartDecorators();
}

void AChart::addSeries(AChartSeries *series, QString nameAxis)
{
  if (m_axisContainer->count() == 0) return;
  AChartAxis *axis;
  if (nameAxis == ACTIV_AXIS) axis = m_axisContainer->activeAxis();
  else axis = m_axisContainer->axis(nameAxis);
  axis->seriesCollection()->AddSeries(series);

  connect(series,SIGNAL(changed()),SLOT(seriesChenged()));
  connect(series,&AChartSeries::destroyed,this,&AChart::removeSeries);
}

AChartSeries *AChart::series(int index,QString nameAxis)
{
  if (m_axisContainer->count() == 0) return NULL;
  AChartAxis *axis;
  if (nameAxis == ACTIV_AXIS) axis = m_axisContainer->activeAxis();
  else axis = m_axisContainer->axis(nameAxis);

  if (index < axis->seriesCollection()->count() && (index >= 0))
    return(axis->seriesCollection()->series(index));
  else return NULL;
}

AChartSeries *AChart::series(QString name, QString nameAxis)
{
  if (m_axisContainer->count() == 0) return NULL;
  AChartAxis *axis;
  if (nameAxis == ACTIV_AXIS) axis = m_axisContainer->activeAxis();
  else axis = m_axisContainer->axis(nameAxis);

  if (axis->seriesCollection()->contains(name)) return(axis->seriesCollection()->series(name));
  else return NULL;
}

AChartAxis *AChart::seriesAxis(AChartSeries *series)
{
  AChartAxis *result = NULL;
  //перебираем оси, находим график
  for (int i = 0;i < m_axisContainer->count();i++)
    {
      if (m_axisContainer->axis(i)->seriesCollection()->contains(series->name()))
        {
          result = m_axisContainer->axis(i);
          break;
        }
    }
  return result;
}

int AChart::seriesCount(QString nameAxis)
{
  AChartAxis *axis;
  if (nameAxis == ACTIV_AXIS) axis = m_axisContainer->activeAxis();
  else axis = m_axisContainer->axis(nameAxis);

  return axis->seriesCollection()->count();
}


void AChart::drawBackground(QPaintDevice *device)
{
  QPainter painter(device);
  painter.fillRect(chartRect(),m_chartBackColor);
  painter.end();
}


void AChart::drawChart(QPaintDevice *device)
{
  //считаем время рисования
#define BAD_DRAWING_TIME 50
  QTime drawingTime;
  QTime calculateTime;


  //рисуем
  QPainter painter(device);
  painter.setRenderHint(m_render);
  QPolygonF seriesPolyLine;
  for (int numAxis = 0;numAxis < m_axisContainer->count();numAxis++)
    {
      AChartAxis *axis = m_axisContainer->axis(numAxis);
      if (!axis->visible()) continue;
      AChartSeriesCollection *seriesCollection = axis->seriesCollection();
      for (int numSeries = 0;numSeries < seriesCollection->seriesCount();numSeries++)
        {
          if (seriesCollection->series(numSeries)->visible())
            {
              calculateTime.start();
              seriesPolyLine = screenPolyline(axis,seriesCollection->series(numSeries));
              if (!seriesPolyLine.isEmpty())
                {
                  painter.setPen(*seriesCollection->series(numSeries)->pen());
                  //ограничиваем область прорисовки нужной частю виджета
                  painter.setClipRect(chartRect(),Qt::ReplaceClip);
                  m_timeCalculate1 += calculateTime.elapsed();
                  drawingTime.start();
                  painter.drawPolyline(seriesPolyLine);
                  m_timeDraw1 += drawingTime.elapsed();
                }
            }
        }
    }
  painter.end();

  //если рисуем долго, замеряем время рисования
  if (drawingTime.elapsed() > BAD_DRAWING_TIME) m_render = QPainter::NonCosmeticDefaultPen;
  else m_render = QPainter::Antialiasing;
}

void AChart::drawBorder(QPaintDevice *device)
{
  QPainter painter(device);
  painter.drawRect(chartRect().adjusted(0, 0, -1, -1));
  painter.end();
}

void AChart::drawAxis(QPaintDevice *device)
{
  QRect area(0,0,device->width(),device->height());
  if (m_horisontralScrollBar->isVisible()) area.adjust(0,0,0,-m_horisontralScrollBar->height());
  if (m_verticalScrollBar->isVisible()) area.adjust(0,0,-m_verticalScrollBar->width(),0);
  setChartRect(m_axisPainter->draw(device,area));
}

void AChart::drawDecorators(QPaintDevice *device)
{
  for (int i=0;i<m_decoratorsContainer.count();i++)
    {
      m_decoratorsContainer[i]->drawEvent(device,chartRect());
    }
}


void AChart::drawMousePhysicalCoord(QPaintDevice *device)
{
  if ((m_mouseX < 0) || (m_mouseY < 0)) return;

  if (m_mousePhysicalCoord.xVisible() or m_mousePhysicalCoord.yVisible())
    {
      QPainter painter(device);
      QPointF realCoord=screenToPhysicalCoords(QPoint(m_mouseX,m_mouseY));
      QPoint mouseCoord=physicalToScreenCoords(realCoord);
      QRect checkRect(chartRect());
      checkRect.adjust(MOUSE_SET_ADJUST_TOP_LEFT,MOUSE_SET_ADJUST_TOP_LEFT,MOUSE_SET_ADJUST_RIGHT_BOTTOM,MOUSE_SET_ADJUST_RIGHT_BOTTOM);
      if (!checkRect.contains(mouseCoord)) return;
      QString xText,yText;
      xText.setNum(realCoord.x(),m_mousePhysicalCoord.xFormat(),m_mousePhysicalCoord.xPrecision());
      yText.setNum(realCoord.y(),m_mousePhysicalCoord.yFormat(),m_mousePhysicalCoord.yPrecision());
      xText="X="+xText;
      yText="Y="+yText;
      if (m_mousePhysicalCoord.xyVisible() && (m_mousePhysicalCoord.xOrientashion()==m_mousePhysicalCoord.yOrientashion()))
        {
          int mouseX=0;
          int mouseY=0;
          QString xyText=xText+" "+yText;
          switch(m_mousePhysicalCoord.xOrientashion())
            {
            case AChartMousePhysicalCoord::ValueOrientashion_bottom:
              mouseX=m_mouseX-fontMetrics().width(xyText)/2;
              mouseY=m_mouseY+fontMetrics().height()*2.5;
              break;

            case AChartMousePhysicalCoord::ValueOrientashion_left:
              mouseX=m_mouseX-(qreal)fontMetrics().width(xyText)-fontMetrics().width('9');
              mouseY=m_mouseY+(qreal)fontMetrics().height()/2;
              break;

            case AChartMousePhysicalCoord::ValueOrientashion_right:
              mouseX=m_mouseX+fontMetrics().width('9')*2;
              mouseY=m_mouseY+(qreal)fontMetrics().height()/2;
              break;

            case AChartMousePhysicalCoord::ValueOrientashion_top:
              mouseX=m_mouseX-fontMetrics().width(xyText)/2;
              mouseY=m_mouseY-fontMetrics().height();
              break;
            };
          painter.setBackgroundMode(Qt::OpaqueMode);
          painter.drawText(mouseX,mouseY,xyText);
        }
      else
        {
          if (m_mousePhysicalCoord.xVisible())
            {
              int mouseX=0;
              int mouseY=0;
              switch(m_mousePhysicalCoord.xOrientashion())
                {
                case AChartMousePhysicalCoord::ValueOrientashion_bottom:
                  mouseX=m_mouseX-fontMetrics().width(yText)/2;
                  mouseY=m_mouseY+fontMetrics().height()*2.5;
                  break;

                case AChartMousePhysicalCoord::ValueOrientashion_left:
                  mouseX=m_mouseX-(qreal)fontMetrics().width(yText)-fontMetrics().width('9');
                  mouseY=m_mouseY+(qreal)fontMetrics().height()/2;
                  break;

                case AChartMousePhysicalCoord::ValueOrientashion_right:
                  mouseX=m_mouseX+fontMetrics().width('9')*2;
                  mouseY=m_mouseY+(qreal)fontMetrics().height()/2;
                  break;

                case AChartMousePhysicalCoord::ValueOrientashion_top:
                  mouseX=m_mouseX-fontMetrics().width(yText)/2;
                  mouseY=m_mouseY-fontMetrics().height();
                  break;
                };
              painter.setBackgroundMode(Qt::OpaqueMode);
              painter.drawText(mouseX,mouseY,xText);
            }
          if (m_mousePhysicalCoord.yVisible())
            {
              int mouseX=0;
              int mouseY=0;
              switch(m_mousePhysicalCoord.yOrientashion())
                {
                case AChartMousePhysicalCoord::ValueOrientashion_bottom:
                  mouseX=m_mouseX-fontMetrics().width(yText)/2;
                  mouseY=m_mouseY+fontMetrics().height()*2.5;
                  break;

                case AChartMousePhysicalCoord::ValueOrientashion_left:
                  mouseX=m_mouseX-(qreal)fontMetrics().width(yText)-fontMetrics().width('9');
                  mouseY=m_mouseY+(qreal)fontMetrics().height()/2;
                  break;

                case AChartMousePhysicalCoord::ValueOrientashion_right:
                  mouseX=m_mouseX+fontMetrics().width('9')*2;
                  mouseY=m_mouseY+(qreal)fontMetrics().height()/2;
                  break;

                case AChartMousePhysicalCoord::ValueOrientashion_top:
                  mouseX=m_mouseX-fontMetrics().width(yText)/2;
                  mouseY=m_mouseY-fontMetrics().height();
                  break;
                };
              painter.setBackgroundMode(Qt::OpaqueMode);
              painter.drawText(mouseX,mouseY,yText);
            }
        }

      painter.end();
    }
}


QRect AChart::chartRect()
{
  return m_chartRect;
}


QPolygon AChart::screenPolyline(AChartAxis *axis, AChartSeries *series)
{        
  QPolygonF *degPoly;
  //    AChartAxis *axis = m_AxisBySeries[series->name()];
  degPoly = series->getOptimalPolyline(this->chartRect().width(),axis->physicalView());
  // перевод координат точек в экранные координаты
  QPolygon scrPoly;
  int pointsCount = degPoly->size();
  if (pointsCount > 0)
    {
      qint64 startI,endI;
      qreal viewMinX = axis->viewMinX();
      qreal viewMaxX = axis->viewMaxX();
      qreal viewMinY = axis->viewMinY();
      qreal viewWidth = axis->viewMaxX() - axis->viewMinX();
      qreal viewHeight = axis->viewMaxY() - axis->viewMinY();
      //если график нормальный, то быстро находим начальную и конечную точки
      if (series->isNormalTrend())
        {
          startI = optimalSeriesIndexOf(degPoly,viewMinX);
          endI = optimalSeriesIndexOf(degPoly,viewMaxX);
          //        startI = series->drawIndexOf(viewMinX);
          //        endI = series->drawIndexOf(viewMaxX);
        }
      else
        {
          startI=0;
          endI=pointsCount;
        }

      QPoint prevPoint = QPoint();
      for (qint64 i = startI; i < endI; i++) {
          qreal pointX = QPointF(degPoly->at(i)).x();
          qreal pointY = QPointF(degPoly->at(i)).y();
          // проверка выхода за границы
          if (i < (pointsCount - 1))
            if (QPointF(degPoly->at(i + 1)).x() < viewMinX)
              continue;
          if (i > 0) if (QPointF(degPoly->at(i - 1)).x() > viewMaxX) continue;
          // перевести
          qreal dx = pointX - viewMinX;
          qreal dy = pointY - viewMinY;

          int screenX = qRound(chartRect().left() + (dx * (chartRect().width()-1) / viewWidth));
          int screenY = qRound(chartRect().bottom() - (dy * (chartRect().height()-1) / viewHeight));
          //убираем одинаковые точки
          QPoint point(screenX, screenY);
          if ( (i == startI) || (point != prevPoint)) scrPoly.append(point);
          prevPoint = point;
        }
    }

  // адаптивная деградация сжатых участков кривой ©
  // ...

  return scrPoly;
}

qreal AChart::viewMinX() const
{
  return m_axisContainer->activeAxis()->viewMinX();
}

qreal AChart::viewMinY() const
{
  return m_axisContainer->activeAxis()->viewMinY();
}

qreal AChart::viewMaxX() const
{
  return m_axisContainer->activeAxis()->viewMaxX();
}

qreal AChart::viewMaxY() const
{
  return m_axisContainer->activeAxis()->viewMaxY();
}

qreal AChart::viewWidth() const
{
  return m_axisContainer->activeAxis()->viewWidth();
}

qreal AChart::viewHeight() const
{
  return m_axisContainer->activeAxis()->viewHeight();
}

void AChart::paintEvent(QPaintEvent *event)
{
  event->accept();//чтобы не писало сообщения о неиспользуемой переменной
  if (ViewMode() != ViewMode_Fixed) changePhisicalViewByViewMode();
  drawChartToDevice(this);
  drawMousePhysicalCoord(this);
}

void AChart::resizeEvent(QResizeEvent *event)
{
  event->accept();//чтобы не писало сообщения о неиспользуемой переменной
  if (m_resizeOnScrool)
    {refreshUnitsPerPixel();
      //подгоняем размеры
#define SLIDER_SIZE 20
      m_verticalScrollBar->setGeometry(width()-SLIDER_SIZE,0,SLIDER_SIZE,height());
      m_horisontralScrollBar->setGeometry(0,height()-SLIDER_SIZE,width()-m_verticalScrollBar->width(),SLIDER_SIZE);

      //  setChartMargins();
    }
}

void AChart::movePhysicalView(qreal dx, qreal dy)
{
  //  m_physicalView.translate(-dx, -dy);
  //  m_currentAxis->setPhysicalView(m_physicalView);
  m_axisContainer->activeAxis()->movePhysicalView(dx,dy);
  physicalViewChangeEvent();
}

void AChart::setPhysicalView(qreal minX, qreal minY, qreal maxX, qreal maxY)
{
  //  m_physicalView.setCoords(minX, minY, maxX, maxY);
  m_axisContainer->activeAxis()->setPhysicalView(minX, minY, maxX, maxY);
  //  m_currentAxis->setPhysicalView(m_physicalView);
  refreshUnitsPerPixel();
  physicalViewChangeEvent();
}

void AChart::setPhysicalView(QDateTime startTime, QDateTime endTime)
{
  m_axisContainer->activeAxis()->setPhysicalView(startTime,endTime);
}

QRectF AChart::physicalView()
{
  //  return(m_physicalView);
  return m_axisContainer->activeAxis()->physicalView();
}

void AChart::setViewMode(ViewModes mode)
{
  ViewModes oldMode = m_viewMode;
  m_viewMode = mode;
  if (mode != oldMode) emit viewModeChanged(m_viewMode);
  m_savedViewMode = mode;
}

void AChart::userActionChangeViewMode(bool mode)
{
  m_userActionChengeViewMode = mode;
}

bool AChart::isUserActionChangeViewMode()
{
  return(m_userActionChengeViewMode);
}

void AChart::setSliderVisibleMode(AChart::ScrollBarVisibleMode mode)
{
  m_scrollBarVisibleMode=mode;
  sliderViewControl();
  //  setChartMargins();
}

QColor AChart::chartBackColor()
{
  return m_chartBackColor;
}

void AChart::setChartBackColor(QColor newColor)
{

  m_chartBackColor = newColor;
}

//QColor AChart::axisBackColor()
//{
//  return m_chartAxicsBackColor;
//}

//void AChart::setAxisBackColor(QColor newColor)
//{
//   m_chartAxicsBackColor = newColor;
//}

QPointF AChart::screenToPhysicalCoords(QPoint screenCoords, AChartAxis *axis)
{
  if (axis == NULL) axis = m_axisContainer->activeAxis();
  float xKoef=(axis->viewMaxX() - axis->viewMinX())/chartRect().width();
  float yKoef=(axis->viewMaxY() - axis->viewMinY())/chartRect().height();
  float x = axis->viewMinX() + (screenCoords.x() - chartRect().left()) * xKoef;
  float y = axis->viewMinY() + (chartRect().height() - (screenCoords.y() - chartRect().top() + 1)) * yKoef;
  return (QPointF(x,y));
}

QPoint AChart::physicalToScreenCoords(QPointF realCoords, AChartAxis *axis)
{
  if (axis == NULL) axis = m_axisContainer->activeAxis();
  float xKoef = (axis->viewMaxX() - axis->viewMinX())/(chartRect().width());
  float yKoef = (axis->viewMaxY() - axis->viewMinY())/chartRect().height();
  int x = qRound(chartRect().left() + (realCoords.x() - axis->viewMinX()) / xKoef);
  int y = qRound(chartRect().height() + (chartRect().top() - 1) - (realCoords.y() - axis->viewMinY()) / yKoef);
  return (QPoint(x,y));
}

//QPointF AChart::screenToPhysicalCoords(QPoint screenCoords)
//{
//  return(screenToPhysicalCoords(screenCoords,m_currentPaintDevice));
//}

//QPoint AChart::physicalToScreenCoords(QPointF realCoords)
//{
//  return(physicalToScreenCoords(realCoords,m_currentPaintDevice));
//}

qreal AChart::screenToPhysicalX(int screenX, AChartAxis *axis)
{
  if (axis == NULL) axis = m_axisContainer->activeAxis();
  float xKoef = (axis->viewMaxX() - axis->viewMinX()) / chartRect().width();
  float x = axis->viewMinX() + (screenX - chartRect().left()) * xKoef;
  return x;
}

int AChart::physicalToScreenX(qreal realX, AChartAxis *axis)
{
  if (axis == NULL) axis = m_axisContainer->activeAxis();
  float xKoef = (axis->viewMaxX() - axis->viewMinX()) / (chartRect().width());
  int x = qRound(chartRect().left() + (realX - axis->viewMinX()) / xKoef);
  return x;
}

qreal AChart::screenToPhysicalY(int screenY, AChartAxis *axis)
{
  if (axis == NULL) axis = m_axisContainer->activeAxis();
  float yKoef = (axis->viewMaxY() - axis->viewMinY()) / chartRect().height();
  float y = axis->viewMinY() + (chartRect().height() - (screenY - chartRect().top() + 1)) * yKoef;
  return y;
}

int AChart::physicalToScreenY(qreal realY, AChartAxis *axis)
{
  if (axis == NULL) axis = m_axisContainer->activeAxis();
  float yKoef = (axis->viewMaxY() - axis->viewMinY()) / chartRect().height();
  int y = qRound(chartRect().height() + (chartRect().top() -1) - (realY - axis->viewMinY()) / yKoef);
  return y;
}

qreal AChart::screenToPhysicalWidth(int screenWidth, AChartAxis *axis)
{
  if (axis == NULL) axis = m_axisContainer->activeAxis();
  float xKoef = (axis->viewMaxX() - axis->viewMinX()) / (chartRect().width());
  return screenWidth * xKoef;
}

int AChart::physicalToScreenWidth(qreal physicalWidth, AChartAxis *axis)
{
  if (axis == NULL) axis = m_axisContainer->activeAxis();
  float xKoef = (axis->viewMaxX() - axis->viewMinX()) / (chartRect().width());
  return qRound(physicalWidth / xKoef);
}

qreal AChart::screenToPhysicalHeight(int screenHeight, AChartAxis *axis)
{
  if (axis == NULL) axis = m_axisContainer->activeAxis();
  float yKoef = (axis->viewMaxY() - axis->viewMinY()) / chartRect().height();
  return qRound(screenHeight * yKoef);
}

int AChart::physicalToScreenHeight(qreal physicalHeight, AChartAxis *axis)
{
  if (axis == NULL) axis = m_axisContainer->activeAxis();
  float yKoef = (axis->viewMaxY() - axis->viewMinY()) / chartRect().height();
  return qRound(physicalHeight / yKoef);
}

void AChart::drawChartToDevice(QPaintDevice *device)
{
  QTime fullTime;
  fullTime.start();
  m_timeFull1 = 0;
  m_timeDraw1 = 0;
  m_timeCalculate1 = 0;
  m_currentPaintDevice = device;
  drawBackground(device);
  drawAxis(device);
  drawChart(device);
  drawBorder(device);
  if (m_decoratorsVisible) drawDecorators(device);
  m_timeFull1 = fullTime.elapsed();
  m_timeFull = m_timeFull1;
  m_timeDraw = m_timeDraw1;
  m_timeCalculate = m_timeCalculate1;
  QPainter paint(m_currentPaintDevice);
  paint.drawText(0,0,QString::number(1/m_timeFull));

}

void AChart::setAxisPainter(AChartAxisPainter *axisPainter)
{
  m_axisPainter->deleteLater();
  m_axisPainter = axisPainter;
}

AChartAxisPainter *AChart::axisPainter()
{
  return m_axisPainter;
}

AChartSeriesCollection *AChart::seriesCollection(QString nameAxis)
{
  AChartAxis *axis;
  if (nameAxis == ACTIV_AXIS) axis = m_axisContainer->activeAxis();
  else axis = m_axisContainer->axis(nameAxis);
  return(axis->seriesCollection());
}

AChartAxisContainer *AChart::axisContainer()
{
  return m_axisContainer;
}

void AChart::addChartDecorator(AChartDecorator *decorator)
{
  m_decoratorsContainer << decorator;
  //  m_savedViewMode = m_viewMode;
  //  setViewMode(ViewMode_Fixed);
  connect(decorator,&AChartDecorator::destroyed,this,&AChart::removeChartDecorator);
  //  connect(decorator,&AChartDecorator::objectBuildered,this,&AChart::onDecoratorBuildered);
}

void AChart::removeChartDecorator(AChartDecorator *decorator)
{
  m_decoratorsContainer.removeOne(decorator);
}

void AChart::removeChartDecorators()
{
  for (int i = 0;i < m_decoratorsContainer.count();i++)
    {
      m_decoratorsContainer[i]->deleteLater();
    }
  m_decoratorsContainer.clear();
}

void AChart::setDecoratorsVisible(bool visible)
{
  m_decoratorsVisible = visible;
}

bool AChart::decoratorsVisible()
{
  return m_decoratorsVisible;
}

AChart::ViewModes AChart::ViewMode()
{
  return(m_viewMode);
}

void AChart::setViewModeMargins(int x, int y)
{
  m_viewModeMarginsX = x;
  m_viewModeMarginsY = y;
}

void AChart::refreshUnitsPerPixel()
{
  if (!isVisible()) return;
  m_physUnitPerPixelX = viewWidth() / qreal(chartRect().width());
  m_physUnitPerPixelY = viewHeight() / qreal(chartRect().height());
}

// Функция должна вызываться при добовлении новой точки к любому графику
// в зависимости от заданного типа визуализации и изменяем окно просмотра графиков
void AChart::changePhisicalViewByViewMode()
{
  // если нет ни одного графика то модифицировать окно не по чем
  if (axisContainer()->count() == 0) return;

  switch (ViewMode())
    {
    //всегда показывать последнюю точку
    case ViewMode_AlwaysViewEnd:
      showLastPointH();
      showLastPointV();
      break;

    case ViewMode_AlwaysViewEndOnlyByX:
      showLastPointH();
      break;

    case ViewMode_AlwaysViewEndOnlyByY:
      showLastPointV();
      break;

      //не изменять окно просмотра
    case ViewMode_Fixed:
      break;

      //весь график на экране
    case ViewMode_allOnScreen:
      for (int numAxis = 0;numAxis < m_axisContainer->count();numAxis++)
        {
          AChartAxis *axis = m_axisContainer->axis(numAxis);
          AChartSeriesCollection *seriesCollection = axis->seriesCollection();
          axis->setPhysicalView(seriesCollection->minX(),seriesCollection->minY(),seriesCollection->maxX(),seriesCollection->maxY());
        }

      break;

    case ViewMode_heightAsMaxPhisicalHeigth:
      for (int numAxis = 0;numAxis < m_axisContainer->count();numAxis++)
        {
          AChartAxis *axis = m_axisContainer->axis(numAxis);
          AChartSeriesCollection *seriesCollection = axis->seriesCollection();
          setPhysicalView(axis->viewMinX(),seriesCollection->minY(),axis->viewMaxX(),seriesCollection->maxY());
        }
      break;

    case ViewMode_widthAsMaxPhisicalWidth:
      for (int numAxis = 0;numAxis < m_axisContainer->count();numAxis++)
        {
          AChartAxis *axis = m_axisContainer->axis(numAxis);
          AChartSeriesCollection *seriesCollection = axis->seriesCollection();
          setPhysicalView(seriesCollection->minX(),axis->viewMinY(),seriesCollection->maxX(),axis->viewMaxY());
        }
      break;
    };
}

void AChart::showLastPointH()
{
  //проверяем надо ли сдвигать оси по X и сдвигаем их
  for (int numAxis = 0;numAxis < m_axisContainer->count();numAxis++)
    {
      AChartAxis *axis = m_axisContainer->axis(numAxis);
      if (!axis->seriesCollection()->isAllSeriesEmpty())
        {
          qreal maxX = axis->seriesCollection()->maxX();
          qreal physHMargins = screenToPhysicalWidth(m_viewModeMarginsX,axis);
          qreal predel = axis->viewMaxX() - physHMargins;
          if (maxX > predel)
            {
              maxX += physHMargins;
              //делаем так,чтобы окно просмотра сдвигалось только кратно 1 экранной точке
              qreal kratnost = screenToPhysicalWidth(1,axis);
              qreal add = kratnost - fmod(maxX,kratnost) - kratnost;// (-kratnost) патаму, чтобы в начале полоса прокрутки не дёргалась
              maxX += add;

              qreal startX = maxX - axis->viewWidth();
              axis->setPhysicalView(startX,axis->viewMinY(),maxX,axis->viewMaxY());
            }
        }
    }
}

void AChart::showLastPointV()
{
  for (int numAxis = 0;numAxis < m_axisContainer->count();numAxis++)
    {
      AChartAxis *axis = m_axisContainer->axis(numAxis);
      AChartSeriesCollection *seriesCollection = axis->seriesCollection();
      if (!seriesCollection->isAllSeriesEmpty())
        {
          //получаем максимальное и манимальное значение среди последних точек графика
          bool firstMinMaxFinded = false;
          qreal minY = 0;
          qreal maxY = 0;
          for (int i = 0;i<seriesCollection->seriesCount();i++)
            {
              AChartSeries *series = seriesCollection->series(i);
              if (series->pointsCount() == 0 ) continue;
              if (series->lastPoint().x() < viewMinX()) continue;
              if (firstMinMaxFinded)
                {
                  if (series->lastPoint().y() < minY) minY = series->lastPoint().y();
                  if (series->lastPoint().y() > maxY) maxY = series->lastPoint().y();
                }
              else
                {
                  minY = series->lastPoint().y();
                  maxY = series->lastPoint().y();
                  firstMinMaxFinded = true;
                }
            }
          qreal physVMargins = screenToPhysicalHeight(m_viewModeMarginsY,axis);
          qreal minPredel = axis->viewMinY() + physVMargins;
          qreal maxPredel = axis->viewMaxY() - physVMargins;
          //если эти значения выходят за пределы окна просмотра то сдвигаем окно просмотра

          //если хоть один из графиков не пустой
          if (firstMinMaxFinded)
            {
              //если разница между максимальным и минимальным значением больше окна просмотра, то непонятно куда
              //его сдвигать
              if ( (maxY-minY) < (axis->physicalView().height() - physVMargins * 2) )
                {
                  if (maxY > maxPredel)
                    {
                      qreal heighY = maxY + physVMargins;
                      qreal lowY = heighY - axis->physicalView().height();
                      axis->setPhysicalView(axis->viewMinX(),lowY,axis->viewMaxX(),heighY);
                    }
                  if (minY < minPredel)
                    {
                      qreal lowY = minY - physVMargins;
                      qreal heighY = lowY + axis->physicalView().height();
                      axis->setPhysicalView(axis->viewMinX(),lowY,axis->viewMaxX(),heighY);
                    }
                }
            }
        }
    }
}
float AChart::timeDraw() const
{
  return m_timeDraw;
}

float AChart::timeFull() const
{
  return  m_timeFull;
}

float AChart::timeCalculate() const
{
  return m_timeCalculate;
}

void AChart::mousePressEvent(QMouseEvent *event)
{
  if (m_mouseMoveAxis)
    {
      //если попадаем в область построения графика
      if (chartRect().contains(event->pos()))
        {
          bool decotatorTakeSignal = sendDecoratorsMousePressEvent(event);
          if (decotatorTakeSignal) return;

          if (event->button() == Qt::RightButton) {
              m_mouseRightPressed = true;
              setChartMode( AChart::ChartMode_MovingView );
            }

          if (event->button() == Qt::LeftButton) m_mouseLeftPressed = true;
        }
      else//в противном случае отправляем запрос рисовальщику осей
        {
          m_axisPainter->mousePressEvent(event);
        }

      m_mousePrevX = m_mouseX;
      m_mousePrevY = m_mouseY;
    }
}

void AChart::mouseReleaseEvent(QMouseEvent *event)
{
  if (m_mouseMoveAxis)
    {
      //если попадаем в область построения графика
      if (chartRect().contains(event->pos()))
        {
          bool decotatorTakeSignal = sendDecoratorsMouseReleaseEvent(event);
          if (decotatorTakeSignal) return;

          if (event->button() == Qt::RightButton) {
              m_mouseRightPressed = false;
              setChartMode( AChart::ChartMode_View );
            }

          if (event->button() == Qt::LeftButton) m_mouseLeftPressed = false;
        }
      else//в противном случае отправляем запрос рисовальщику осей
        {
          m_axisPainter->mouseReleaseEvent(event);
        }
    }
}

void AChart::mouseMoveEvent(QMouseEvent *event)
{
  if (m_mouseMoveAxis)
    {
      //если попадаем в область построения графика
      if (chartRect().contains(event->pos()))
        {
          bool decotatorTakeSignal = sendDecoratorsMouseMoveEvent(event);
          if (decotatorTakeSignal)
            {
              userChangePhisicalView();
              seriesChenged();
              return;
            }

          m_mouseX = event->x();
          m_mouseY = event->y();

          if (chartMode() == ChartMode_MovingView)
            {
              int deltaX = m_mouseX - m_mousePrevX;
              int deltaY = m_mouseY - m_mousePrevY;

              for (int i=0;i<m_axisContainer->count();i++)
                {
                  AChartAxis *axis = m_axisContainer->axis(i)/*m_axisContainer->next()*/;
                  qreal moveX = 0;
                  qreal moveY = 0;
                  // если это текущая ось или мы изменяем все оси по X
                  if ((axis == m_axisContainer->activeAxis()) || m_mouseChangeAllAxisByX)
                    {
                      qreal physPerPixel = axis->viewWidth() / chartRect().width();
                      moveX = qreal(deltaX) * physPerPixel;
                    }
                  // если это текущая ось или мы изменяем все оси по Y
                  if ((axis == m_axisContainer->activeAxis()) || m_mouseChangeAllAxisByY)
                    {
                      qreal physPerPixel = axis->viewHeight() / chartRect().height();
                      moveY = -qreal(deltaY) * physPerPixel;
                    }
                  m_axisContainer->axis(i)->movePhysicalView(moveX, moveY);
                }
              userChangePhisicalView();
              seriesChenged();
            }


          this->update();
          m_mousePrevX = m_mouseX;
          m_mousePrevY = m_mouseY;
        }
      else//в противном случае отправляем запрос рисовальщику осей
        {
          m_axisPainter->mouseMoveEvent(event);
        }
    }
}

void AChart::wheelEvent(QWheelEvent *event)
{
  //если попадаем в область построения графика
  if (chartRect().contains(event->pos()))
    {
      //если декораторы приняли сигнал, то дальше он не должен идти
      bool decotatorTakeSignal = sendDecoratorsWheelEvent(event);
      if (decotatorTakeSignal)
        {
          userChangePhisicalView();
          update();
          return;
        }

      for (int i = 0;i < m_axisContainer->count();i++)
        {
          //          aChartAxis *axis = m_axisContainer->activeAxis();
          AChartAxis *axis = m_axisContainer->axis(i);
          qreal numDegrees = event->delta() / 8;
          qreal numSteps = numDegrees / 15;
          qreal mouseX = event->x() - chartRect().left();
          qreal koefX = (qreal)mouseX / chartRect().width();

          qreal prcX1 = (m_wheelZoomPercentX * koefX)  / 100;
          qreal prcX2 = (m_wheelZoomPercentX * (1 - koefX))  / 100;

          qreal zoomX1 = numSteps * axis->viewWidth() * prcX1;
          qreal zoomX2 = numSteps * axis->viewWidth() * prcX2;
          qreal x1 = axis->viewMinX() + zoomX1;
          qreal x2 = axis->viewMaxX() - zoomX2;


          qreal mouseY = (qreal)chartRect().height() - ( event->y() - chartRect().top() );
          qreal koefY = (qreal)mouseY / chartRect().height();

          qreal prcY1 = (m_wheelZoomPercentY * koefY)  / 100;
          qreal prcY2 = (m_wheelZoomPercentY * (1 - koefY))  / 100;

          qreal zoomY1 = numSteps * axis->viewHeight() * prcY1;
          qreal zoomY2 = numSteps * axis->viewHeight() * prcY2;
          qreal y1 = axis->viewMinY() + zoomY1;
          qreal y2 = axis->viewMaxY() - zoomY2;

          if (event->modifiers()==Qt::NoModifier)
            {
              if (axis == m_axisContainer->activeAxis())
                axis->setPhysicalView(x1, y1, x2, y2);
              else
                {
                  if ( m_mouseChangeAllAxisByX && m_mouseChangeAllAxisByY)
                    axis->setPhysicalView(x1, y1, x2, y2);
                  if ( m_mouseChangeAllAxisByX && !m_mouseChangeAllAxisByY)
                    axis->setPhysicalView(x1, axis->viewMinY(), x2, axis->viewMaxY());
                  if ( !m_mouseChangeAllAxisByX && m_mouseChangeAllAxisByY)
                    axis->setPhysicalView(axis->viewMinX(), y1, axis->viewMaxX(), y2);
                }
            }
          if (event->modifiers()==Qt::ControlModifier)
            {
              if ( (axis == m_axisContainer->activeAxis()) || m_mouseChangeAllAxisByY)
                axis->setPhysicalView(axis->viewMinX(), y1, axis->viewMaxX(), y2);
            }
          if (event->modifiers()==Qt::ShiftModifier)
            {
              if ( (axis == m_axisContainer->activeAxis()) || m_mouseChangeAllAxisByX)
                axis->setPhysicalView(x1, axis->viewMinY(), x2, axis->viewMaxY());
            }
        }

      userChangePhisicalView();

      this->update();
    }
  else//в противном случае отправляем запрос рисовальщику осей
    {
      m_axisPainter->wheelEvent(event);
    }
}

void AChart::leaveEvent(QEvent *event)
{
  if (m_mouseRightPressed) setChartMode( AChart::ChartMode_View );
  m_mouseRightPressed = false;
  m_mouseLeftPressed = false;
  m_mouseX = -1;
  m_mouseY = -1;
  update();
  event->accept();
}

void AChart::setMouseCoordsPaint(bool enable)
{
  m_mousePhysicalCoord.setXY_visible(enable);
}

bool AChart::sendDecoratorsMousePressEvent(QMouseEvent *event)
{
  for (int i=0; i<m_decoratorsContainer.count();i++)
    {
      if (m_decoratorsContainer[i]->mousePressEvent(event)) return true;
    }
  return false;
}

bool AChart::sendDecoratorsMouseReleaseEvent(QMouseEvent *event)
{
  for (int i=0; i<m_decoratorsContainer.count();i++)
    {
      if (m_decoratorsContainer[i]->mouseReleaseEvent(event)) return true;
    }
  return false;
}

bool AChart::sendDecoratorsMouseMoveEvent(QMouseEvent *event)
{
  for (int i=0; i<m_decoratorsContainer.count();i++)
    {
      if (m_decoratorsContainer[i]->mouseMoveEvent(event)) return true;
    }
  return false;
}

bool AChart::sendDecoratorsWheelEvent(QWheelEvent *event)
{
  for (int i=0; i<m_decoratorsContainer.count();i++)
    {
      if (m_decoratorsContainer[i]->wheelEvent(event)) return true;
    }
  return false;
}

//ищем точку с заданной X координатой методом половинного деления
int AChart::optimalSeriesIndexOf(QPolygonF *poligon, qreal x)
{
  if (poligon->isEmpty()) return 0;
  if  (x < poligon->at(0).x()) return 0;
  if (x > poligon->at(poligon->count() - 1).x()) return poligon->count();
  int leftI = 0;
  int rightI = poligon->count() - 1;
  while ((rightI - leftI) > 3)
    {
      int middleI = leftI + (rightI - leftI) / 2;
      if ( x < poligon->at(middleI).x() )
        rightI = middleI;
      else
        leftI = middleI;
    }
  //ещем минимально отличающуюся точку
  qreal minDelta = qAbs(x - poligon->at(leftI).x());
  int index = leftI;
  for (int i = leftI;i < rightI;i++)
    {
      qreal delta = qAbs(x - poligon->at(i).x());
      if (delta <= minDelta)
        {
          index = i;
          minDelta = delta;
        }
    }
  return index;
}

void AChart::setWheelZoomPercent(qreal x, qreal y)
{
  m_wheelZoomPercentX = x;
  m_wheelZoomPercentY = y;
}

void AChart::createFormComponent()
{
#define SLIDER_SIZE 20
  m_verticalScrollBar = new QScrollBar(Qt::Vertical,this);
  m_verticalScrollBar->setGeometry(width()-SLIDER_SIZE,0,SLIDER_SIZE,height());
  m_verticalScrollBar->setRange(0,1000);
  connect(m_verticalScrollBar,SIGNAL(actionTriggered(int)),this,SLOT(onVerticalScrollBarMoved(int)));

  m_horisontralScrollBar= new QScrollBar(Qt::Horizontal,this);
  m_horisontralScrollBar->setGeometry(0,height()-SLIDER_SIZE,width()-m_verticalScrollBar->width(),SLIDER_SIZE);
  m_horisontralScrollBar->setRange(0,1000);
  connect(m_horisontralScrollBar,SIGNAL(actionTriggered(int)),this,SLOT(onHorisontalScrollBarMoved(int)));

}

void AChart::sliderViewControl()
{
  switch (m_scrollBarVisibleMode)
    {
    case sliderVisibleMode_alwaysVisible:
      m_horisontralScrollBar->show();
      m_verticalScrollBar->show();
      break;

    case sliderVisibleMode_alwaysInVisible:
      m_horisontralScrollBar->hide();
      m_verticalScrollBar->hide();
      break;

    case sliderVisibleMode_isNeedVisible:
      //если хоть один из графиков выходит за пределы окна просмотра то ScrollBar должен быть видимым
      bool hVisible = false;
      bool vVisible = false;
      for (int i = 0;i < m_axisContainer->count();i++)
        {
          AChartAxis *axis = m_axisContainer->axis(i);
          AChartSeriesCollection *seriesCollection = axis->seriesCollection();
          if(seriesCollection->minX() < axis->viewMinX() or seriesCollection->maxX() > axis->viewMaxX())
            hVisible = true;
          if(seriesCollection->minY() < axis->viewMinY() or seriesCollection->maxY() > axis->viewMaxY())
            vVisible = true;
        }
      m_horisontralScrollBar->setVisible(hVisible);
      m_verticalScrollBar->setVisible(vVisible);
      //        if(m_axisContainer->minX()<viewMinX() or m_seriesCollection.maxX()>viewMaxX()) m_horisontralScrollBar->show();
      //        else m_horisontralScrollBar->hide();
      //      if(m_seriesCollection.minY()<viewMinY() or m_seriesCollection.maxY()>viewMaxY()) m_verticalScrollBar->show();
      //      else m_verticalScrollBar->hide();
      break;
    };
}

void AChart::setChartMode(ChartModes mode)
{
  m_chartMode = mode;
}
bool AChart::mouseMoveAxis() const
{
  return m_mouseMoveAxis;
}

void AChart::setMouseMoveAxis(bool mouseMoveAxis)
{
  m_mouseMoveAxis = mouseMoveAxis;
}

bool AChart::resizeOnScrool() const
{
  return m_resizeOnScrool;
}

void AChart::setResizeOnScrool(bool resizeOnScrool)
{
  m_resizeOnScrool = resizeOnScrool;
}

bool AChart::mouseChangeAllAxisByX() const
{
  return m_mouseChangeAllAxisByX;
}

void AChart::setMouseChangeAllAxisByX(bool mouseChangeAllAxisByX)
{
  m_mouseChangeAllAxisByX = mouseChangeAllAxisByX;
}

bool AChart::mouseChangeAllAxisByY() const
{
  return m_mouseChangeAllAxisByY;
}

void AChart::setMouseChangeAllAxisByY(bool mouseChangeAllAxisByY)
{
  m_mouseChangeAllAxisByY = mouseChangeAllAxisByY;
}


void AChart::seriesChenged()
{
  //если пришло новое значение от любого графика и давненько не перерисовывали, то перерисовываем
#define DELTA_TIME_REPAINT 50
  if (isHidden()) return;
  if (m_axisContainer->count() == 0) return;

  if (m_repaintTime->elapsed() > DELTA_TIME_REPAINT)
    {
      sliderViewControl();
      update();
      m_repaintTime->restart();
    }

  //если пользователь не двигает полосой прокрутки, то в зависимости от pазмера графика рассчитываем положение полосы прокрутки
  if (!m_horisontralScrollBar->isSliderDown())
    {
      // запоминаем соотношение начал осей
      qreal minAxisStartX = m_axisContainer->axis(0)->viewMinX();
      qreal minAxisStartY = m_axisContainer->axis(0)->viewMinY();
      for (int i = 0;i < m_axisContainer->count();i++)
        {
          if (m_axisContainer->axis(i)->viewMinX() < minAxisStartX) minAxisStartX = m_axisContainer->axis(i)->viewMinX();
          if (m_axisContainer->axis(i)->viewMinY() < minAxisStartY) minAxisStartY = m_axisContainer->axis(i)->viewMinY();
        }
      qreal maxDx = 0;
      qreal maxDy = 0;
      for (int i = 0; i < m_axisContainer->count();i++)
        {
          qreal deltaX = m_axisContainer->axis(i)->viewMinX() - minAxisStartX;
          qreal deltaY = m_axisContainer->axis(i)->viewMinY() - minAxisStartY;
          if (maxDx < deltaX) maxDx = deltaX;
          if (maxDy < deltaY) maxDy = deltaY;
        }
      //пересчитывем координаты
      qreal deltaH = (maxDx + (m_axisContainer->maxX()-m_axisContainer->minX()) )/(m_horisontralScrollBar->maximum()-m_horisontralScrollBar->minimum());
      qreal deltaV = (maxDy + (m_axisContainer->maxY()-m_axisContainer->minY()) )/(m_verticalScrollBar->maximum()-m_verticalScrollBar->minimum());
      m_horisontralScrollBar->setValue(floor(minAxisStartX/deltaH));
      m_verticalScrollBar->setValue(m_verticalScrollBar->maximum()-floor((minAxisStartY + maxDy - m_axisContainer->minY())/deltaV));
    }
  //--------
}

void AChart::onHorisontalScrollBarMoved(int typeSignal)
{
  //  if (!m_horisontralScrollBar->isSliderDown()) return;
  if (m_chartMode == ChartMode_MovingView) return;//чтобы небыло зацикливания
  if (typeSignal == QAbstractSlider::SliderNoAction) return;
  // запоминаем соотношение начал осей
  qreal minAxisStartX = m_axisContainer->axis(0)->viewMinX();
  for (int i = 0;i < m_axisContainer->count();i++)
    if (m_axisContainer->axis(i)->viewMinX() < minAxisStartX) minAxisStartX = m_axisContainer->axis(i)->viewMinX();
  QList<qreal> dX;
  qreal maxDx=0;
  for (int i = 0; i < m_axisContainer->count();i++)
    {
      qreal deltaX = m_axisContainer->axis(i)->viewMinX() - minAxisStartX;
      if (maxDx < deltaX) maxDx = deltaX;
      dX << deltaX;
    }
  //пересчитываем координаты
  for (int i = 0; i < m_axisContainer->count();i++)
    {
      float deltaH = (maxDx + (m_axisContainer->maxX() - m_axisContainer->minX()) ) / (m_horisontralScrollBar->maximum() - m_horisontralScrollBar->minimum());
      qreal minX = (qreal)m_horisontralScrollBar->value() * deltaH + dX[i] - maxDx;
      qreal delta = m_axisContainer->axis(i)->viewWidth();
      qreal maxX = minX + delta;
      m_axisContainer->axis(i)->setPhysicalView(minX,m_axisContainer->axis(i)->viewMinY(),maxX,m_axisContainer->axis(i)->viewMaxY());
    }
  userChangePhisicalView();
  update();
}

void AChart::onVerticalScrollBarMoved(int typeSignal)
{
  //  if (!m_verticalScrollBar->isSliderDown()) return;
  if (m_chartMode == ChartMode_MovingView) return;//чтобы небыло зацикливания
  if (typeSignal == QAbstractSlider::SliderNoAction) return;
  // запоминаем соотношение начал осей
  qreal minAxisStartY = m_axisContainer->axis(0)->viewMinY();
  for (int i = 0;i < m_axisContainer->count();i++)
    if (m_axisContainer->axis(i)->viewMinY() < minAxisStartY) minAxisStartY = m_axisContainer->axis(i)->viewMinY();
  QList<qreal> dY;
  qreal MaxDy = 0;
  for (int i = 0; i < m_axisContainer->count();i++)
    {
      qreal deltaAxisY = m_axisContainer->axis(i)->viewMinY() - minAxisStartY;
      if (deltaAxisY > MaxDy) MaxDy = deltaAxisY;
      dY << deltaAxisY;
    }
  //пересчитываем координаты
  //  typeSignal = m_verticalScrollBar->maximum() - typeSignal;
  int value = m_verticalScrollBar->maximum() - m_verticalScrollBar->value();
  for (int i = 0; i < m_axisContainer->count();i++)
    {
      qreal deltaV = (MaxDy + (m_axisContainer->maxY()-m_axisContainer->minY()))/(m_verticalScrollBar->maximum()-m_verticalScrollBar->minimum());
      qreal minY = m_axisContainer->minY() + (qreal)value * deltaV + dY[i] - MaxDy;
      qreal delta = m_axisContainer->axis(i)->viewHeight();
      qreal maxY = minY + delta;
      m_axisContainer->axis(i)->setPhysicalView(m_axisContainer->axis(i)->viewMinX(),minY,m_axisContainer->axis(i)->viewMaxX(),maxY);
    }
  userChangePhisicalView();
  update();
}

//void AChart::onDecoratorBuildered()
//{
////  m_viewMode = m_savedViewMode;
//}

void AChart::removeSeries(AChartSeries *series)
{
  seriesAxis(series)->seriesCollection()->remove(series->name());
}

AChart::ChartModes AChart::chartMode() const
{
  return m_chartMode;
}

void AChart::userChangePhisicalView()
{
  if (m_userActionChengeViewMode) setViewMode(ViewMode_Fixed);
  sliderViewControl();
}

void AChart::physicalViewChangeEvent()
{
}

void AChart::setChartRect(QRect chartRect)
{
  m_chartRect = chartRect;
}
