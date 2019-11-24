#include "achartaxis.h"
#include <QDateTime>

AChartAxis::AChartAxis(QObject *parent) :
  QObject(parent)
{
  m_physicalView.setCoords(0, 0, 10, 10);
  m_visible = true;
}

AChartAxis::AChartAxis(qreal minX, qreal minY, qreal maxX, qreal maxY, QString name, QColor color, QObject *parent) :
  QObject(parent)
{
  m_physicalView.setCoords(minX, minY, maxX, maxY);
  m_name = name;
  m_color = color;
  m_visible = true;
}

QRectF AChartAxis::physicalView()
{
  return m_physicalView;
}

qreal AChartAxis::viewMinX() const
{
    return m_physicalView.x();
}

qreal AChartAxis::viewMinY() const
{
    return m_physicalView.y();
}

qreal AChartAxis::viewMaxX() const
{
    return m_physicalView.x() + m_physicalView.width();
}

qreal AChartAxis::viewMaxY() const
{
    return m_physicalView.y() + m_physicalView.height();
}

qreal AChartAxis::viewWidth() const
{
    return m_physicalView.width();
}

qreal AChartAxis::viewHeight() const
{
  return m_physicalView.height();
}

void AChartAxis::setPhysicalView(qreal minX, qreal minY, qreal maxX,qreal maxY)
{
  m_physicalView.setCoords(minX, minY, maxX, maxY);
}

void AChartAxis::setPhysicalView(QDateTime startTime, QDateTime endTime)
{


    m_physicalView.setX(startTime.toMSecsSinceEpoch());
    m_physicalView.setWidth(endTime.toMSecsSinceEpoch()-startTime.toMSecsSinceEpoch());
}

void AChartAxis::movePhysicalView(qreal dX, qreal dY)
{
  m_physicalView.translate(-dX, -dY);
}

void AChartAxis::setPhysicalView(QRectF physicalView)
{
  m_physicalView = physicalView;
}

QString AChartAxis::name()
{
  return m_name;
}

void AChartAxis::setName(QString name)
{
  m_name = name;
}

QColor AChartAxis::color()
{
  return m_color;
}

void AChartAxis::setColor(QColor color)
{
  m_color = color;
}
AChartSeriesCollection *AChartAxis::seriesCollection()
{
    return &m_seriesCollection;
}
bool AChartAxis::visible() const
{
    return m_visible;
}

void AChartAxis::setVisible(bool visible)
{
    m_visible = visible;
}



