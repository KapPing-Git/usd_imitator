#ifndef ACHARTRECTSELECTOR_H
#define ACHARTRECTSELECTOR_H

#include <QObject>
#include "../achartdecorator.h"
#include <QTextCodec>
#include <QMenu>

class AChartRectSelector : public AChartDecorator
{
  Q_OBJECT
public:
  explicit AChartRectSelector(AChart *parent = 0);
  void toStartStage();
  QString typeName();
  QByteArray data();
  void setData(QByteArray *byteArray);
  AChartDecorator *cloneMyType(AChart *chart);

  QRectF areaRect() const;
  void setAreaRect(const QRectF &areaRect);
  int borderWidth() const;
  void setBorderWidth(int borderWidth);
  QColor areaColor() const;
  void setAreaColor(const QColor &areaColor);
  QPen areaPen() const;
  void setAreaPen(const QPen &areaPen);
  QPen borderPen() const;
  void setBorderPen(const QPen &borderPen);

signals:

public slots:
protected:
  void mousePress(QMouseEvent *event);
  void mouseRelease(QMouseEvent *event);
  void mouseMove(QMouseEvent *event);
  void wheel(QWheelEvent *event);

  bool isMouseMoveTransperency();
  bool isMousePressTransperency();
  bool isMouseReleaseTransperency();
  bool isWheelTransperency();

  void draw(QPaintDevice *device,QRect chartRect);
  bool containsScreenPoint(QPoint point);
  bool isCanDraw();

private:
#define CURRENT_VERSION (quint16)0
  enum stageEnum {stage_complete,
                  stage_firtsPointSetting,
                  stage_twoPointSetting,
                  stage_leftMoving,
                  stage_rightMoving,
                  stage_topMoving,
                  stage_bottomMoving,
                  stage_moving};
  stageEnum m_stage;

  bool m_menuReady;
  QMenu menu;
  QAction *delAction;

  QPoint m_forMenuPointBegin;
  QPoint m_forMenuPointEnd;
  bool m_borderVisible;
  QColor m_areaColor;
  int m_transparency;
  QPen m_areaPen;
  QPen m_borderPen;
  QPointF m_firstPoint,m_secondPoint;
  QPointF m_deltaToTopLeft;
  qreal m_deltaRight;
  qreal m_deltaTop;
  qreal m_deltaBottom;
  QPointF topLeft();
  QPointF bottomRight();
  QRectF m_areaRect;
  QRect screenRect();
  QRect leftRect();
  QRect rightRect();
  QRect topRect();
  QRect bottomRect();

  int m_borderWidth;
  bool m_containScreenPoint;

};

#endif // ACHARTRECTSELECTOR_H
