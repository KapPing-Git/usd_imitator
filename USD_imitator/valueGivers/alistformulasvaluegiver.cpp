#include "alistformulasvaluegiver.h"

AListFormulasValueGiver::AListFormulasValueGiver(AIntervalContainer *intervalContainer, qreal frecuency, bool autoRepeate, QObject *parent)
  : AAbstractValueGiver(frecuency,autoRepeate ,parent)
{
  m_intervalContainer = intervalContainer;
  setAutoRepeate(autoRepeate);
  m_currTime = 0;
  m_numCurrentFormula = 0;
}

AListFormulasValueGiver::~AListFormulasValueGiver()
{

}
AIntervalContainer *AListFormulasValueGiver::intervalContainer() const
{
  return m_intervalContainer;
}

void AListFormulasValueGiver::setIntervalContainer(AIntervalContainer *intervalContainer)
{
  m_intervalContainer = intervalContainer;
}

void AListFormulasValueGiver::start()
{
  AAbstractValueGiver::start();
  m_currTime = 0;
  m_timeInterval = (1/frecuency());
  m_numCurrentFormula = 0;
}

void AListFormulasValueGiver::DoNextValue()
{
  // находим нужный интервал
  AInterval interval;
  while (m_numCurrentFormula < m_intervalContainer->count())
    {
      interval = m_intervalContainer->at(m_numCurrentFormula);
      if ( (m_currTime >= interval.begin()) && (m_currTime < interval.end()) )
        break;
      else
        m_numCurrentFormula++;
    }
  //если интервалы не закончились
  if (m_numCurrentFormula < m_intervalContainer->count())
    {

      // по формуле выдаём нужное значение
      m_engine.globalObject().setProperty("t",m_currTime - interval.begin());
      qreal y = m_engine.evaluate(interval.formula()).toNumber();
      emit nextValue(y);

      // если нет автоповторения то останавливаем иначе повторяем с начала
      m_currTime += m_timeInterval;
    }
  else
    {
      if (autoRepeate())
        {
          m_numCurrentFormula = 0;
          m_currTime = 0;
        }
      else
        {
          stop();
        }
    }
}


