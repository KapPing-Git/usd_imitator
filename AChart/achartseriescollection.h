#ifndef ACHARTSERIESCOLLECTION_H
#define ACHARTSERIESCOLLECTION_H

#include <QHash>
#include <QString>
#include <QDataStream>

#include "achartseries.h"
#include "achartpolyline.h"

class AChartSeriesCollection : public QHash<QString, AChartSeries *>
{
   // Q_OBJECT
public:
  explicit AChartSeriesCollection();

  void AddSeries(AChartSeries *series);

  AChartSeries*   series(int index);
  AChartSeries*   series(QString name);
  int             seriesCount();
  void setData(QByteArray data);
  QByteArray data();

  AChartPolyline*  seriesPolyline(int seriesIndex);
  bool isAllSeriesEmpty();
  qreal minX();
  qreal minY();
  qreal maxX();
  qreal maxY();

signals:

public slots:


private:
#define VERSION (quint16)0

};

#endif // ACHARTSERIESCOLLECTION_H
