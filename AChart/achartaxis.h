#ifndef ACHARTAXIS_H
#define ACHARTAXIS_H

#include <QObject>
#include <QColor>
#include <QRectF>
#include "achartseriescollection.h"

class AChartAxis : public QObject
{
  Q_OBJECT
public:
  explicit AChartAxis(QObject *parent = 0);
  explicit AChartAxis(qreal minX, qreal minY, qreal maxX, qreal maxY,QString name,QColor color,QObject *parent = 0);
  QRectF physicalView();
  qreal viewMinX() const;
  qreal viewMinY() const;
  qreal viewMaxX() const;
  qreal viewMaxY() const;
  qreal viewWidth() const;
  qreal viewHeight() const;
  void setPhysicalView(qreal minX, qreal minY, qreal maxX, qreal maxY);
  void setPhysicalView(QDateTime startTime,QDateTime endTime);
  void movePhysicalView(qreal dX, qreal dY);
  void setPhysicalView(QRectF physicalView);

  QString name();
  void setName(QString name);

  QColor color();
  void setColor(QColor color);

  AChartSeriesCollection *seriesCollection();

  bool visible() const;
  void setVisible(bool visible);

signals:
  
public slots:

private:
  QColor m_color;
  QString m_name;
  QRectF m_physicalView;
  AChartSeriesCollection m_seriesCollection;
  bool m_visible;
};

#endif // ACHARTAXIS_H
