#ifndef ACHART_H
#define ACHART_H

#include <QWidget>
#include <QPainter>
#include <QPaintDevice>
#include <QColor>
#include <QMargins>
#include <QRect>
#include <QRectF>
#include <QPolygon>
#include <QPoint>
#include <QPointF>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QSlider>
#include <QScrollBar>

#include "achartseries.h"
#include "achartseriescollection.h"
#include "achartmousephysicalcoord.h"
#include "achartaxiscollection.h"
#include "AChartAxisPainter/achartaxispainter.h"
#include "ACHartDevice/achartdecorator.h"

class AChartDecorator;
class AChartAxisPainter;

class AChart : public QWidget
{
    Q_OBJECT
public:
  explicit AChart(QWidget *parent = 0);
  ~AChart();
#define STD_AXIS "standartAxis"
#define ACTIV_AXIS "activeAxis"
  void addSeries(AChartSeries *series,QString nameAxis = ACTIV_AXIS);
  AChartSeries *series(int index,QString nameAxis = ACTIV_AXIS);
  AChartSeries *series(QString name,QString nameAxis = ACTIV_AXIS);
  AChartAxis *seriesAxis(AChartSeries *series);
  int seriesCount(QString nameAxis = ACTIV_AXIS);
  void setPhysicalView(qreal minX, qreal minY, qreal maxX, qreal maxY);
  void setPhysicalView(QDateTime startTime,QDateTime endTime);
  QRectF physicalView();
  qreal viewMinX() const;
  qreal viewMinY() const;
  qreal viewMaxX() const;
  qreal viewMaxY() const;
  qreal viewWidth() const;
  qreal viewHeight() const;
  //определяет как зависят размеры отображаемой области от минимальных и максимальных значений трендов
  enum ViewModes {ViewMode_Fixed,//никак не зависят
                  ViewMode_allOnScreen,//все графики видно полностью
                  ViewMode_heightAsMaxPhisicalHeigth,//все графики видно полностью по вертикали
                  ViewMode_widthAsMaxPhisicalWidth,//все графики видно полностью по горизонтали
                  ViewMode_AlwaysViewEnd,//всегда виден конец графика
                  ViewMode_AlwaysViewEndOnlyByX,//следим за концом только по горизонтали
                  ViewMode_AlwaysViewEndOnlyByY//следим за концом только по вертикали
                 };
  ViewModes ViewMode();
  void setViewModeMargins(int x,int y);

  QMargins margins();
  //------

  void userActionChangeViewMode(bool mode);
  bool isUserActionChangeViewMode();

  //определяем порядок видимости полос прокрутки
  enum ScrollBarVisibleMode {sliderVisibleMode_alwaysVisible,
                          sliderVisibleMode_alwaysInVisible,
                          sliderVisibleMode_isNeedVisible
                         };
  void setSliderVisibleMode(ScrollBarVisibleMode mode);
  //------------

  //Определяем тип заднего фона
  QColor chartBackColor();
  void setChartBackColor(QColor newColor);

//  //? необходимо сделать тип заднего фона

  //преобразования координат мыши в физические координаты
  QPointF screenToPhysicalCoords(QPoint screenCoords,AChartAxis* axis = NULL);
  QPoint physicalToScreenCoords(QPointF realCoords,AChartAxis* axis = NULL);
  qreal screenToPhysicalX(int screenX,AChartAxis* axis = NULL);
  int physicalToScreenX(qreal realX,AChartAxis* axis = NULL);
  qreal screenToPhysicalY(int screenY,AChartAxis* axis = NULL);
  int physicalToScreenY(qreal realY,AChartAxis* axis = NULL);
  qreal screenToPhysicalWidth(int screenWidth,AChartAxis* axis = NULL);
  int physicalToScreenWidth(qreal physicalWidth,AChartAxis* axis = NULL);
  qreal screenToPhysicalHeight(int screenHeight,AChartAxis* axis = NULL);
  int physicalToScreenHeight(qreal physicalHeight,AChartAxis* axis = NULL);
  //------

  virtual void drawChartToDevice(QPaintDevice *device);

  void setAxisPainter(AChartAxisPainter *axisPainter);
  AChartAxisPainter *axisPainter();
  AChartAxisContainer *axisContainer();
  AChartSeriesCollection *seriesCollection(QString nameAxis = ACTIV_AXIS);

  //методы работы с внешними устройствами
  void addChartDecorator(AChartDecorator *decorator);
  void removeChartDecorator(AChartDecorator *decorator);
  void removeChartDecorators();
  void setDecoratorsVisible(bool visible);
  bool decoratorsVisible();
  //----

  bool mouseChangeAllAxisByY() const;
  void setMouseChangeAllAxisByY(bool mouseChangeAllAxisByY);
  bool mouseChangeAllAxisByX() const;
  void setMouseChangeAllAxisByX(bool mouseChangeAllAxisByX);
  void setMouseCoordsPaint(bool enable);

  bool resizeOnScrool() const;
  void setResizeOnScrool(bool resizeOnScrool);


  bool mouseMoveAxis() const;
  void setMouseMoveAxis(bool mouseMoveAxis);

  float timeCalculate() const;
  float timeDraw() const;
  float timeFull() const;


signals:
  void viewModeChanged(ViewModes viewMode);

public slots:
  void setViewMode(ViewModes mode);

private slots:
  void seriesChenged();
  void onHorisontalScrollBarMoved(int typeSignal);
  void onVerticalScrollBarMoved(int typeSignal);
//  void onDecoratorBuildered();

private:
#define MOUSE_SET_ADJUST_RIGHT_BOTTOM -10
#define MOUSE_SET_ADJUST_TOP_LEFT 5

  AChartMousePhysicalCoord m_mousePhysicalCoord;
  void removeSeries(AChartSeries *series);

  enum ChartModes { ChartMode_View, ChartMode_MovingView };
  void setChartMode(ChartModes mode);
  ChartModes  m_chartMode;
  bool m_mouseChangeAllAxisByY;
  bool m_mouseChangeAllAxisByX;

  bool m_resizeOnScrool;
  bool m_mouseMoveAxis;

  ChartModes  chartMode() const;


  QColor  m_chartBackColor;
  qreal   m_physUnitPerPixelX;
  qreal   m_physUnitPerPixelY;

  void    movePhysicalView(qreal dx, qreal dy);
  void    refreshUnitsPerPixel();

  void changePhisicalViewByViewMode();
  void showLastPointH();
  void showLastPointV();

  /*время для отладки рисования и просчитиывания точек мс */
  float m_timeDraw;
  float m_timeCalculate;
  float m_timeFull;
  float m_timeDraw1;
  float m_timeCalculate1;
  float m_timeFull1;

protected:

  AChartAxisContainer *m_axisContainer;
  AChartAxisPainter *m_axisPainter;
  QHash<QString,AChartAxis *> m_AxisBySeries;

  virtual void    drawBackground(QPaintDevice *device);
  virtual void    drawChart(QPaintDevice *device);
  virtual void    drawBorder(QPaintDevice *device);
  virtual void    drawAxis(QPaintDevice *device);
  void            drawDecorators(QPaintDevice *device);
  void  drawMousePhysicalCoord(QPaintDevice *device);


  virtual void userChangePhisicalView();
  virtual void physicalViewChangeEvent();
  QRect m_chartRect;
  void setChartRect(QRect chartRect);
  QRect chartRect();

  void paintEvent(QPaintEvent *event);
  void resizeEvent(QResizeEvent *event);

  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  void leaveEvent(QEvent *event);

  bool sendDecoratorsMousePressEvent(QMouseEvent *event);
  bool sendDecoratorsMouseReleaseEvent(QMouseEvent *event);
  bool sendDecoratorsMouseMoveEvent(QMouseEvent *event);
  bool sendDecoratorsWheelEvent(QWheelEvent *event);
private:

  int optimalSeriesIndexOf(QPolygonF *poligon, qreal x);
  QPolygon    screenPolyline(AChartAxis *axis, AChartSeries *series);

  int m_mouseX;
  int m_mouseY;
  bool m_mouseRightPressed;
  bool m_mouseLeftPressed;
  int m_mousePrevX;
  int m_mousePrevY;
  QPaintDevice *m_currentPaintDevice;


  qreal m_wheelZoomPercentX;
  qreal m_wheelZoomPercentY;
  void setWheelZoomPercent(qreal x, qreal y);     // процент масштабирования при использовании колеса мышки

  QTime *m_repaintTime;

  ViewModes m_viewMode;
  ViewModes m_savedViewMode;
  int m_viewModeMarginsX;
  int m_viewModeMarginsY;
  ScrollBarVisibleMode m_scrollBarVisibleMode;
  bool m_userActionChengeViewMode;

  //визуальные компоненты
  void createFormComponent();
  QScrollBar *m_verticalScrollBar;
  QScrollBar *m_horisontralScrollBar;
  void sliderViewControl();

  QList<AChartDecorator *> m_decoratorsContainer;
  bool m_decoratorsVisible;

  QPainter::RenderHint m_render;


};

#endif // ACHART_H
