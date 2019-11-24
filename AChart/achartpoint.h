#ifndef ACHARTPOINT_H
#define ACHARTPOINT_H

#include <QObject>
#include <QPointF>

class AChartPoint : public QPointF
{
 //   Q_OBJECT
public:
    explicit AChartPoint();
    AChartPoint(qreal x, qreal y);

signals:

public slots:

};

#endif // ACHARTPOINT_H
