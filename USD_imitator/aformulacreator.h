#ifndef FORMULACREATOR_H
#define FORMULACREATOR_H

#include <QWidget>
#include <QtScript>
#include <QList>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>

#include "aintervalshower.h"
#include "aintervalcontainer.h"
#include "ainserter.h"
#include "../AChart/achart.h"
#include "../AChart/achartseries.h"

namespace Ui {
  class AFormulaCreator;
}

class AFormulaCreator : public QWidget
{
  Q_OBJECT

public:
  explicit AFormulaCreator(QWidget *parent = 0);
  ~AFormulaCreator();

  AIntervalContainer intervals();
  void setIntervals(AIntervalContainer intervals);

  qreal frecuency() const;
  void setFrecuency(const qreal &frecuency);

  QByteArray data();
  void setData(QByteArray *data);

  void clear();

signals:
  void changed();

private slots:
  void onInserterPressed(AInserter *inserter);
  void onIntervalRemoved(int numInterval);
  void onIntervalTimeEditFinished(int numInterval);
  void onIntervalFormulaEditFinished(int numInterval);
  void onIntervalSelected(AIntervalShower *intervalShower);
  void onIntervalFormulaChanged(QString formula);
  void onCurrformulaEditTextChanged();
  void onNameChanged(QString name);
  void onOkButtonClicked();
  void onCreateGraphButtonClicked();

private:
  Ui::AFormulaCreator *ui;
  AIntervalContainer m_intervals;
  QPointer<QScrollArea> m_scrollArea;
  QList<AIntervalShower *> m_intervalShowers;
  QVBoxLayout m_mainLayout;
  QHBoxLayout m_intervalsLayout;
  AChart *m_chart;
  AChartSeries *m_series;
  qreal m_frecuency;
  QScriptEngine m_engine;
  QLineEdit *m_formulaName;
  QLabel *m_formulaNameLabel;
  QTextEdit *m_currformulaEdit;
  QPushButton *m_okButton;
  QPushButton *m_createGraphButton;
  QHBoxLayout m_mainButtonLayout;
  AIntervalShower *m_currentIntervalShower;

  bool m_destroying;
  void intervalShowersUpdate();
  void updateTrend();
  void reBuild();
};

#endif // FORMULACREATOR_H
