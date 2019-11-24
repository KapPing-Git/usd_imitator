#ifndef ACHARTAXISPAINTER_H
#define ACHARTAXISPAINTER_H

#include <QObject>
#include <../AChart/achartaxis.h>
#include <../AChart/achartaxiscollection.h>
#include <../AChart/achart.h>
#include <QPaintDevice>
#include <QRect>

class AChart;
class AChartAxisPainter : public QObject
{
  Q_OBJECT
public:
  explicit AChartAxisPainter(AChart *chart);
  //функция должна возвращать область для рисования графика
  virtual QRect draw(QPaintDevice *paintDevice,QRect paintRect) = 0;
  virtual void mousePressEvent(QMouseEvent *event) = 0;
  virtual void mouseReleaseEvent(QMouseEvent *event) = 0;
  virtual void mouseMoveEvent(QMouseEvent *event) = 0;
  virtual void wheelEvent(QWheelEvent *event) = 0;
  QColor backColor() const;
  void setBackColor(const QColor &backColor);

  char xFormat() const;
  void setXFormat(char xFormat);
  char yFormat() const;
  void setYFormat(char yFormat);
  int xPrecision() const;
  void setXPrecision(int xPrecision);
  int yPrecision() const;
  void setYPrecision(int yPrecision);

  QSize gridSize() const;
  void setGridSize(const QSize &gridSize);

signals:

public slots:

protected:
  AChart *m_chart;
  QColor m_backColor;

  char m_xFormat;
  char m_yFormat;
  int  m_xPrecision;
  int  m_yPrecision;
  QSize m_gridSize;
};

#endif // ACHARTAXISPAINTER_H
