#ifndef AABSTRACTVALUEGIVER_H
#define AABSTRACTVALUEGIVER_H

#include <QObject>
#include <QTimer>

class AAbstractValueGiver : public QObject
{
  Q_OBJECT
public:
  explicit AAbstractValueGiver(qreal frecuency = 0, bool autoRepeate = false, QObject *parent = 0);
  ~AAbstractValueGiver();

  virtual void start();
  virtual void stop();
  bool isActive();

  bool autoRepeate() const;
  void setAutoRepeate(bool autoRepeate);

  qreal frecuency() const;
  void setFrecuency(const qreal &frecuency);

signals:
  void nextValue(qreal value);
  void started();
  void stoped();

public slots:

protected:
  virtual void DoNextValue() = 0;

private:
  bool m_autoRepeate;
  qreal m_frecuency;
  QTimer m_timer;

};

#endif // AABSTRACTVALUEGIVER_H
