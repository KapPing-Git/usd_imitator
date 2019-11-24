#include "ainterval.h"

AInterval::AInterval(qreal startTime, qreal length, QString formula)
{
  m_length = length;
  m_formula = formula;
  m_startTime = startTime;
}

qreal AInterval::length() const
{
  return m_length;
}

void AInterval::setLength(const qreal &length)
{
  m_length = length;
}
QString AInterval::formula() const
{
  return m_formula;
}

void AInterval::setFormula(const QString &formula)
{
  m_formula = formula;
}

qreal AInterval::begin()
{
  return m_startTime;
}

qreal AInterval::end()
{
  return m_startTime + m_length;
}

QByteArray AInterval::data()
{
  QByteArray byteArray;
  QDataStream in(&byteArray,QIODevice::WriteOnly);
  in << m_startTime << m_length << m_formula;
  return byteArray;
}

void AInterval::setData(QByteArray *data)
{
  QDataStream out(data,QIODevice::ReadOnly);
  out >> m_startTime >> m_length >> m_formula;
}
qreal AInterval::startTime() const
{
  return m_startTime;
}

void AInterval::setStartTime(const qreal &startTime)
{
  m_startTime = startTime;
}
