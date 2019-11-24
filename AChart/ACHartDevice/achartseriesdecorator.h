#ifndef ACHARTSERIESDECORATOR_H
#define ACHARTSERIESDECORATOR_H

#include "achartdecorator.h"
#include "../achartseries.h"
#include <QPointer>

class AChartSeriesDecorator : public AChartDecorator
{
  Q_OBJECT
public:
  explicit AChartSeriesDecorator(AChart *parent,AChartSeries * series,qreal beginX = 0, qreal endX = 0);
  explicit AChartSeriesDecorator(AChart *parent,int numSeries,qreal beginX = 0, qreal endX = 0);
  explicit AChartSeriesDecorator(AChart *parent,QString nameSeries,qreal beginX = 0, qreal endX = 0);
  AChartSeries *series();
  qreal beginX();
  qreal endX();
  QString seriesName();
  
signals:
  
public slots:

protected:
  QPointer<AChartSeries> m_series;
  qreal m_beginX;
  qreal m_endX;
  bool m_complete;
  QString m_savedNameAxis;
  void beforeAnySignal();
  void afterAnySignal();

private:
  void deleteI();
  
};

#endif // ACHARTSERIESDECORATOR_H
