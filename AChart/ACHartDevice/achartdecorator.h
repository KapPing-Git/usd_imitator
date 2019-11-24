#ifndef ACHARTDEVICE_H
#define ACHARTDEVICE_H

#include <QObject>
#include <../AChart/achart.h>
#include <QMouseEvent>
#include <QPaintDevice>
#include <QPoint>
#include <QByteArray>

class AChart;

class AChartDecorator : public QObject
{
  Q_OBJECT
public:
  AChartDecorator(AChart *parent);
  ~AChartDecorator();
  virtual QString typeName() = 0;
  void drawEvent(QPaintDevice *device,QRect chartRect);
  //следующие функции возвращают true, мышь попала в область декоратора
  bool mousePressEvent(QMouseEvent *event);
  bool mouseReleaseEvent(QMouseEvent *event);
  bool mouseMoveEvent(QMouseEvent *event);
  bool wheelEvent(QWheelEvent *event);
  //-----
  virtual QByteArray data() = 0;
  virtual void setData(QByteArray *byteArray) = 0;
  friend void operator<<(QDataStream & stream, AChartDecorator & decorator);
  friend void operator>>(QDataStream & stream, AChartDecorator & decorator);
  virtual AChartDecorator *cloneMyType(AChart *chart) = 0;

  bool visible() const;
  void setVisible(bool visible);

signals:
  void destroyed(AChartDecorator *decorator);
  void objectBuildered();
  void objectChanged();

  
public slots:

protected:  
  virtual void mousePress(QMouseEvent *event) = 0;
  virtual void mouseRelease(QMouseEvent *event) = 0;
  virtual void mouseMove(QMouseEvent *event) = 0;
  virtual void wheel(QWheelEvent *event) = 0;
  virtual void beforeAnySignal();
  virtual void afterAnySignal();
  virtual bool isMousePressTransperency();
  virtual bool isMouseReleaseTransperency();
  virtual bool isMouseMoveTransperency();
  virtual bool isWheelTransperency();

  virtual void draw(QPaintDevice *device,QRect chartRect) = 0;
  virtual bool containsScreenPoint(QPoint point) = 0;
  virtual bool isCanDraw() = 0;

  AChart *m_parentChart;
  bool m_visible;

};


#endif // ACHARTDEVICE_H
