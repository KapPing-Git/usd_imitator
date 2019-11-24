#include "aformulaselector.h"
#include "ui_aformulaselector.h"

AFormulaSelector::AFormulaSelector(QString formulaFileName, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AFormulaSelector)
{
  ui->setupUi(this);
  m_groupeName = "";
  m_formulaFileName = formulaFileName;
  setWindowModality(Qt::WindowModal);
  m_formulaCreator = new AFormulaCreator(this);
  m_formulaCreator->setWindowFlags(Qt::Window);
  connect(m_formulaCreator,&AFormulaCreator::changed,this,&AFormulaSelector::onIntervalsChanged);
  m_settings = new QSettings(m_formulaFileName,QSettings::IniFormat,this);
  ui->changeButton->setEnabled(false);
  ui->delButton->setEnabled(false);
  ui->selectButton->setEnabled(false);
}

AFormulaSelector::~AFormulaSelector()
{
  delete ui;
}

AIntervalContainer AFormulaSelector::getFormula(QString groupeName)
{
  m_groupeName = groupeName;

  updateList();
  show();

  //ждём пока пользователь выберет формулу (в этом случае окно закроется)
  while(isVisible())
    {
      QApplication::processEvents();
    }

  return m_intervals;
}

void AFormulaSelector::on_addButton_clicked()
{
  m_formulaCreator->clear();
  m_formulaCreator->show();
}

void AFormulaSelector::on_selectButton_clicked()
{
  m_settings->beginGroup(m_groupeName);
  QString nameFormula = ui->listWidget->currentItem()->text();
  m_intervals.setName(nameFormula);
  QByteArray data = m_settings->value(nameFormula).toByteArray();
  m_intervals.setData(&data);
  m_settings->endGroup();

  close();
}

void AFormulaSelector::onIntervalsChanged()
{
  AIntervalContainer intervals = m_formulaCreator->intervals();
  m_settings->beginGroup(m_groupeName);
  m_settings->setValue(intervals.name(),intervals.data());
  m_settings->endGroup();
  updateList();
}

void AFormulaSelector::on_changeButton_clicked()
{
  if (ui->listWidget->currentItem() != NULL)
    {
      m_settings->beginGroup(m_groupeName);
      QString nameFormula = ui->listWidget->currentItem()->text();
      m_intervals.setName(nameFormula);
      QByteArray data = m_settings->value(nameFormula).toByteArray();
      m_intervals.setData(&data);
      m_settings->endGroup();
      m_formulaCreator->setIntervals(m_intervals);
      m_formulaCreator->show();
    }
}

void AFormulaSelector::on_listWidget_itemPressed(QListWidgetItem *item)
{
  if (item != NULL)
    {
      ui->changeButton->setEnabled(true);
      ui->delButton->setEnabled(true);
      ui->selectButton->setEnabled(true);
    }
  else
    {
      ui->changeButton->setEnabled(false);
      ui->delButton->setEnabled(false);
      ui->selectButton->setEnabled(false);
    }
}

void AFormulaSelector::on_delButton_clicked()
{
  QListWidgetItem *delItem = ui->listWidget->currentItem();
  if (delItem != NULL)
    {
      QMessageBox message;
      message.setText(tr("Вы действительно хотите удалить %1").arg(delItem->text()));
      QPushButton *yesButton = message.addButton(tr("Да"),QMessageBox::YesRole);
      message.addButton(tr("Нет"),QMessageBox::NoRole);
      message.exec();
      QAbstractButton *clickedButton = message.clickedButton();
      if (clickedButton == yesButton)
        {
          m_settings->beginGroup(m_groupeName);
          m_settings->remove(delItem->text());
          m_settings->endGroup();
          updateList();
        }
    }
}

void AFormulaSelector::updateList()
{
  //загружаем данные из файла
  m_settings->beginGroup(m_groupeName);
  m_AllFormulaNames = m_settings->allKeys();
  m_settings->endGroup();
  ui->listWidget->clear();
  ui->listWidget->addItems(m_AllFormulaNames.filter(ui->filterEdit->text()));
  ui->listWidget->addItem(noFormula);
}

void AFormulaSelector::on_filterEdit_textChanged(const QString &arg1)
{
  ui->changeButton->setEnabled(false);
  ui->delButton->setEnabled(false);
  ui->selectButton->setEnabled(false);
  updateList();
}
