#ifndef ACHARTDECORATORCONTAINER_H
#define ACHARTDECORATORCONTAINER_H

#include <QObject>
#include <QList>
#include <QHash>
#include "achartdecorator.h"
#include "../AChart/achart.h"
#include <QByteArray>

//контейнер для декораторов позволяющий сохранять данные из массива декораторов в QbyteArray
//и наоборот по массиву с данными создавать массив декораторов
//Для того чтобы создать массив декораторов из QbyteArray необходимо перед этим
//с помощью addDecoratorForClone добавить по одному декоратору для каждого типа использующихся декораторов
class AChartDecoratorContainer : public QObject
{
  Q_OBJECT
public:
  explicit AChartDecoratorContainer(QObject *parent);
  ~AChartDecoratorContainer();
  void addGroup(QString groupeName);
  QList<AChartDecorator *>* group(QString groupeName);
  void removeGroupe(QString groupeName);
  bool containsGroupe(QString groupeName);
  void addDecorator(QString groupeName,AChartDecorator *decorator);
  void addDecoratorForClone(AChartDecorator *cloneDecorator);
  void removeDecorator(AChartDecorator *decorator);
  QByteArray data();
  void createDecoratorsByData(QByteArray *data,AChart *chart);
  int groupeCount();
  //AChartDecorator *operator[](int index);
//  void decoratorsVisible(bool visible);
  void removeDecoratorsAtAllGroupe();
  void removeAll();
signals:
  void changed();
  
public slots:

private:
  QHash<QString,QList<AChartDecorator *>* > m_groupeDecorators;
  QHash<QString,AChartDecorator *> m_cloneDecorators;
//  bool m_deleting;

  void onDecoratorChanged();
};

#endif // ACHARTDECORATORCONTAINER_H
