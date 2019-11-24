#include "aintervalcontainer.h"

AIntervalContainer::AIntervalContainer() :
  QList<AInterval>()
{
}

QByteArray AIntervalContainer::data()
{
  QByteArray byteArray;
  QDataStream in(&byteArray,QIODevice::WriteOnly);
  for (int i = 0;i < count();i++)
    {
      AInterval interval = at(i);
      in << interval.data();
    }
  return byteArray;
}

void AIntervalContainer::setData(QByteArray *data)
{
  clear();
  //создаём новый список интервалов
  QDataStream out(data,QIODevice::ReadOnly);
  while(!out.atEnd())
    {
      AInterval interval;
      QByteArray data;
      out >> data;
      interval.setData(&data);
      append(interval);
    }
}
QString AIntervalContainer::name() const
{
  return m_name;
}

void AIntervalContainer::setName(const QString &name)
{
  m_name = name;
}

