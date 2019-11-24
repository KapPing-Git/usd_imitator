#include "aabstractvaluegiver.h"

AAbstractValueGiver::AAbstractValueGiver(qreal frecuency, bool autoRepeate, QObject *parent) : QObject(parent)
{
  m_autoRepeate = autoRepeate;
  m_frecuency = frecuency;
  connect(&m_timer,&QTimer::timeout,this,&AAbstractValueGiver::DoNextValue);
}

AAbstractValueGiver::~AAbstractValueGiver()
{

}

void AAbstractValueGiver::start()
{
  if (m_frecuency != 0)
    {
      int timeInterval = qRound(1000 / m_frecuency);
      m_timer.start(timeInterval);
    }
  emit started();
}

void AAbstractValueGiver::stop()
{
  m_timer.stop();
  emit stoped();
}

bool AAbstractValueGiver::isActive()
{
  return m_timer.isActive();
}
bool AAbstractValueGiver::autoRepeate() const
{
  return m_autoRepeate;
}

void AAbstractValueGiver::setAutoRepeate(bool autoRepeate)
{
  m_autoRepeate = autoRepeate;
}
qreal AAbstractValueGiver::frecuency() const
{
  return m_frecuency;
}

void AAbstractValueGiver::setFrecuency(const qreal &frecuency)
{
  m_frecuency = frecuency;
}



