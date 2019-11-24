#ifndef AINTERVALCONTAINER_H
#define AINTERVALCONTAINER_H

#include <QObject>
#include "ainterval.h"

class AIntervalContainer : public QList<AInterval>
{
public:
  explicit AIntervalContainer();

  QByteArray data();
  void setData(QByteArray *data);

  QString name() const;
  void setName(const QString &name);

signals:

public slots:

private:
  QString m_name;

};

#endif // AINTERVALCONTAINER_H
