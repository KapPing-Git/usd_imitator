#ifndef ACHARTAXISCOLLECTION_H
#define ACHARTAXISCOLLECTION_H

#include <QWidget>
#include "achartaxis.h"
#include <QHash>
#include <QHashIterator>

class AChartAxisContainer : public QObject
{
  Q_OBJECT
public:
  AChartAxisContainer(QObject *parent);
  ~AChartAxisContainer();
  void addAxis(qreal minX, qreal minY, qreal maxX, qreal maxY,QString name,QColor color);
  void addAxis(AChartAxis *axis);
  AChartAxis *axis(QString nameAxis);
  AChartAxis *axis(int numAxis);
  void setActiveAxis(QString nameAxis);
  void setActiveAxis(int numAxis);
  AChartAxis *activeAxis();
  int activeAxisNum();
  bool contains(QString nameAxis);
  bool contains(int numAxis);
  void deleteAxis(QString nameAxis);
  void toFront();
  void toBack();
  AChartAxis *next();
  AChartAxis *previos();
  bool hasNext();
  bool hasPrevios();
  int count();
  qreal minX();
  qreal maxX();
  qreal minY();
  qreal maxY();

private slots:
  void onSeriesChanged();

private:
  QHash<QString,AChartAxis *> m_axisCollection;
  AChartAxis *m_activeAxis;
  QHashIterator<QString,AChartAxis *> *m_iterator;

//  qreal m_allSeriesMinX;
//  qreal m_allSeriesMaxX;
//  qreal m_allSeriesMinY;
//  qreal m_allSeriesMaxY;
  
signals:
  
public slots:
  
};

#endif // ACHARTAXIS_H
