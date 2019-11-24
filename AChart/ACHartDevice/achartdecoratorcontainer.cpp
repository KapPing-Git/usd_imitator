#include "achartdecoratorcontainer.h"
#include <QMessageBox>
#include <QTextCodec>

AChartDecoratorContainer::AChartDecoratorContainer(QObject *parent):
  QObject(parent)
{
}

AChartDecoratorContainer::~AChartDecoratorContainer()
{
  for (int i = 0;i < m_cloneDecorators.count();i++)
    {
      delete(m_cloneDecorators.values().at(i));
    }
  removeAll();
}

void AChartDecoratorContainer::addGroup(QString groupeName)
{
  m_groupeDecorators.insert(groupeName, new QList<AChartDecorator*>);
}

QList<AChartDecorator *>* AChartDecoratorContainer::group(QString groupeName)
{
  if (m_groupeDecorators.contains(groupeName)) return m_groupeDecorators[groupeName];
  else return NULL;
}

void AChartDecoratorContainer::removeGroupe(QString groupeName)
{
  QList<AChartDecorator*> *decorators = m_groupeDecorators[groupeName];
  for (int i = 0; i < decorators->count();i++)
    {
      decorators->at(i)->deleteLater();
    }
  decorators->clear();
  m_groupeDecorators.remove(groupeName);
}

bool AChartDecoratorContainer::containsGroupe(QString groupeName)
{
  return m_groupeDecorators.contains(groupeName);
}

void AChartDecoratorContainer::addDecorator(QString groupeName, AChartDecorator *decorator)
{
  //если такой группы нет,то создеём её
  if (!m_groupeDecorators.contains(groupeName))
    m_groupeDecorators.insert(groupeName, new QList<AChartDecorator*>);
  m_groupeDecorators[groupeName]->append(decorator);
  connect(decorator,&AChartDecorator::destroyed,this,&AChartDecoratorContainer::removeDecorator);
  connect(decorator,&AChartDecorator::objectChanged,this,&AChartDecoratorContainer::changed);
}

void AChartDecoratorContainer::addDecoratorForClone(AChartDecorator *cloneDecorator)
{
  m_cloneDecorators.insert(cloneDecorator->typeName(),cloneDecorator);
}

void AChartDecoratorContainer::removeAll()
{
 removeDecoratorsAtAllGroupe();
 for (int numGroupe = 0;numGroupe < m_groupeDecorators.count();numGroupe++)
   {
     QList<AChartDecorator *>* decorators = m_groupeDecorators.values().at(numGroupe);
     delete(decorators);
   }
 m_groupeDecorators.clear();
}

void AChartDecoratorContainer::onDecoratorChanged()
{
  emit changed();
}


void AChartDecoratorContainer::removeDecorator(AChartDecorator *decorator)
{
//  if (!m_deleting) m_decorators.removeOne(decorator);
  for (int numGroupe = 0;numGroupe < m_groupeDecorators.count();numGroupe++)
    {
      QList<AChartDecorator *>* decorators = m_groupeDecorators.values().at(numGroupe);
      decorators->removeOne(decorator);
    }
}

QByteArray AChartDecoratorContainer::data()
{
  //сохраняем группы декораторов
  QByteArray groupeData;
  QDataStream groupeOut(&groupeData,QIODevice::WriteOnly);
  for (int numGroupe = 0; numGroupe < m_groupeDecorators.count();numGroupe++)
    {
      //сохраняем выбранную группу
      QByteArray data;
      QDataStream out(&data,QIODevice::WriteOnly);
      //сохраняем имя группы
      groupeOut << m_groupeDecorators.keys().at(numGroupe);
      //получаем группу декораторов и сохраняем её
      QList<AChartDecorator *>* decorators = m_groupeDecorators.values().at(numGroupe);
      for (int numDecorator = 0; numDecorator < decorators->count();numDecorator++)
        {
          out << decorators->at(numDecorator)->typeName() << *decorators->at(numDecorator);
        }
      groupeOut << data;
    }
  return groupeData;
}

//создаём по данным все группы декораторов
void AChartDecoratorContainer::createDecoratorsByData(QByteArray *data, AChart *chart)
{
  QDataStream groupeIn(data,QIODevice::ReadOnly);
  while (!groupeIn.atEnd())
    {
      // создаём группу
      QString groupeName;
      groupeIn >> groupeName;
      addGroup(groupeName);
      // создаём декораторы
      QByteArray groupeData;
      groupeIn >> groupeData;
      QDataStream in(&groupeData,QIODevice::ReadOnly);
      while (!in.atEnd())
        {
          QString typeName;
          in >> typeName;
          if (m_cloneDecorators.contains(typeName))
            {
              AChartDecorator *decorator = m_cloneDecorators[typeName]->cloneMyType(chart);
              in >> *decorator;
              addDecorator(groupeName,decorator);
//              m_groupeDecorators << decorator;
              connect(decorator,&AChartDecorator::destroyed,this,&AChartDecoratorContainer::removeDecorator);
            }
          else
            {
              QMessageBox::warning(NULL, tr("Вы добавили объект с нужным типом в AChartDecoratorContainer?"),
                                   tr("Воспользуйтесь методом addDecoratorForClone"),
                                   QMessageBox::Ok);
              return;
            }
        }
    }
}

int AChartDecoratorContainer::groupeCount()
{
  return m_groupeDecorators.count();
}

void AChartDecoratorContainer::removeDecoratorsAtAllGroupe()
{
  for (int numGroupe = 0;numGroupe < m_groupeDecorators.count();numGroupe++)
    {
      QList<AChartDecorator *>* decorators = m_groupeDecorators.values().at(numGroupe);
      QListIterator<AChartDecorator *> i(*decorators);
      i.toFront();
      while (i.hasNext())
        {
          i.next()->deleteLater();
        }
//      delete(decorators);
    }
}
