#ifndef AINTERVALSHOWER_H
#define AINTERVALSHOWER_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QtScript>
#include <QPointer>
#include <QMouseEvent>
#include "ainterval.h"

/*
Данный класс описывает формулу действующую на определённом интервале времени
  */
namespace Ui {
  class AIntervalShower;
}

class AIntervalShower : public QWidget
{
  Q_OBJECT

public:
  explicit AIntervalShower(AInterval *interval, int num, QWidget *parent = 0);
  ~AIntervalShower();

  int num();
  void updateValues();
  AInterval *interval();

protected:
  void mouseReleaseEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  bool eventFilter(QObject *target, QEvent *event);

signals:
  void timeIntervalEditFinished(int num);
  void formulaEditFinished(int num);
  void removed(int num);
  void selected(AIntervalShower *intervalShower);
  void formulaChanged(QString formula);

private slots:
  void on_formulaEdit_textChanged(const QString &arg1);
  void on_timeEdit_editingFinished();
  void onDelAction();

  void on_delButton_clicked();

  void on_formulaEdit_editingFinished();

private:
  Ui::AIntervalShower *ui;
  AInterval *m_interval;
  QMenu m_menu;
  QAction *m_delAction;
  QScriptEngine m_engine;
  int m_numAtContainer;
};

#endif // AINTERVAL_H
