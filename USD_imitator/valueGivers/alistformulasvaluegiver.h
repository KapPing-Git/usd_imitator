#ifndef ALISTFORMULASVALUEGIVER_H
#define ALISTFORMULASVALUEGIVER_H

#include <QObject>
#include <valueGivers/aabstractvaluegiver.h>
#include <aintervalcontainer.h>
#include <QtScript>

class AListFormulasValueGiver : public AAbstractValueGiver
{
  Q_OBJECT
public:
  explicit AListFormulasValueGiver(AIntervalContainer *intervalContainer = NULL,qreal frecuency = 0, bool autoRepeate = false,QObject *parent = 0);
  ~AListFormulasValueGiver();

  AIntervalContainer *intervalContainer() const;
  void setIntervalContainer(AIntervalContainer *intervalContainer);

  void start();

signals:

public slots:

protected:
  void DoNextValue();

private:
  AIntervalContainer *m_intervalContainer;
  qreal m_currTime;
  qreal m_timeInterval;
  int m_numCurrentFormula;
  QScriptEngine m_engine;
};

#endif // ALISTFORMULASVALUEGIVER_H
