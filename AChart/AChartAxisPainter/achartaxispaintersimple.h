#ifndef ACHARTAXISPAINTERSIMPLE_H
#define ACHARTAXISPAINTERSIMPLE_H
#include <../AChart/AChartAxisPainter/achartaxispainter.h>

class AChartAxisPainterSimple : public AChartAxisPainter
{
  Q_OBJECT
public:
  explicit AChartAxisPainterSimple(AChart *parent = 0);
  QRect draw(QPaintDevice *paintDevice, QRect paintRect);
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);


signals:

public slots:

private:
  QList<QRect> m_axisRect;

};

#endif // ACHARTAXISPAINTERSIMPLE_H
