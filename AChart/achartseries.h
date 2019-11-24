#ifndef ACHARTSERIES_H
#define ACHARTSERIES_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QByteArray>
#include <QDateTime>

#include "achartpoint.h"
#include "achartpolyline.h"

class AChartSeries : public QObject
{
  Q_OBJECT
public:
  explicit AChartSeries(QObject *parent,QString name);
  ~AChartSeries();

  AChartPolyline* polyline() const;
  // кол-во точек на один пиксель ширины
  AChartPolyline *getOptimalPolyline(int screenWidth, QRectF physicalView);
  virtual void addPoint(qreal x,qreal y);
  virtual void addPoint(QDateTime dt,qreal value);
  void setPoint(int numPoint,qreal x,qreal y);
  void setPointCount(int count);
  void updateOptimalPolylineInfo(int screenWidth, QRectF physicalView);
  QPointF point(int index);
  QPointF lastPoint();
  void setData(QByteArray *data);
  void clear();
  void startTransaction();
  void stopTransaction();
  void setPen(QPen *pen);
  QPen *pen();
  qreal minX();
  qreal minY();
  qreal maxX();
  qreal maxY();
  qreal averageDeltaX();
  bool isNormalTrend();
  int indexOf(qreal x);

  int pointsCount();

  QString name();
  QByteArray *data();

  bool visible() const;
  void setVisible(bool visible);

  qreal intervalMin(qreal beginX,qreal endX);
  qreal intervalMax(qreal beginX,qreal endX);
  qreal intervalAverage(qreal beginX,qreal endX);

signals:
  void changed();
  void destroyed(AChartSeries *series);

public slots:
  virtual void upgrade();


private:
  int m_cashedScreenWidth ;
  QRectF m_cashedPhysicalView ;

  bool equalASPercent(qreal firstVal,qreal secondVal,qreal percent);

protected:
  AChartPolyline*   m_realPolyline;
  AChartPolyline*   m_optimalPolyline;
  QString m_name;
  bool m_useOptimal;
  bool m_transaction;
  qreal m_minX;
  qreal m_minY;
  qreal m_maxX;
  qreal m_maxY;
  QPointF m_lastPointMax,m_lastPointMin;
  int m_screenPerPhysX,m_currScreenX,m_startX;

  QPen *m_Pen;
  bool m_normalTrend;//если каждое следующее значение
  bool m_visible;

  virtual void onUpgrade();

#define CURRENT_VERSION (quint16)0
};

#endif // ACHARTSERIES_H
