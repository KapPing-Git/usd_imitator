#ifndef AINTERVAL_H
#define AINTERVAL_H

#include <QtScript>
#include <QPointer>

class AInterval
{
public:
  explicit AInterval(qreal startTime = 0,qreal length = 0, QString formula = "");

  qreal length() const;
  void setLength(const qreal &length);
  QString formula() const;
  void setFormula(const QString &formula);
  qreal begin();
  qreal end();
  QByteArray data();
  void setData(QByteArray *data);
  qreal startTime() const;
  void setStartTime(const qreal &startTime);

private:
  qreal m_length;
  QString m_formula;
  qreal m_startTime;

};

#endif // AINTERVAL_H
