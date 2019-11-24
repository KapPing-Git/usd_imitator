#ifndef AFORMULASELECTOR_H
#define AFORMULASELECTOR_H

#include <QWidget>
#include <QFileDialog>
#include <QSettings>
#include <QListWidgetItem>
#include <QMessageBox>

#include "aintervalcontainer.h"
#include "aformulacreator.h"

namespace Ui {
  class AFormulaSelector;
}

class AFormulaSelector : public QWidget
{
  Q_OBJECT

public:

#define noFormula tr("Формула отсутствует")

  explicit AFormulaSelector(QString formulaFileName, QWidget *parent = 0);
  ~AFormulaSelector();

  AIntervalContainer getFormula(QString groupeName);

private slots:
  void on_addButton_clicked();
  void on_selectButton_clicked();

  void onIntervalsChanged();

  void on_changeButton_clicked();

  void on_listWidget_itemPressed(QListWidgetItem *item);

  void on_delButton_clicked();

  void on_filterEdit_textChanged(const QString &arg1);

private:
  Ui::AFormulaSelector *ui;
  AFormulaCreator *m_formulaCreator;
  QString m_formulaFileName;
  QString m_groupeName;
  AIntervalContainer m_intervals;
  QSettings *m_settings;
  QStringList m_AllFormulaNames;

  void updateList();
};

#endif // AFORMULASELECTOR_H
