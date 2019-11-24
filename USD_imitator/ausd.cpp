#include "ausd.h"

AUsd::AUsd(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AUsd)
{
  ui->setupUi(this);
  setFrecuency(15);
  m_formulaSelector = new AFormulaSelector(mainSettingsPath() + QDir::separator() + "formulas.ini",this);
  m_formulaSelector->setWindowFlags(Qt::Window);
  m_series = new AChartSeries(this,"aScan");
  ui->chart->addSeries(m_series);
  ui->chart->setViewMode(AChart::ViewMode_Fixed);
  ui->chart->setPhysicalView(0,0,800,120);
  ui->chart->setSliderVisibleMode(AChart::sliderVisibleMode_alwaysInVisible);
  ui->powerSlider->setMinimum(1);
  ui->powerSlider->setMaximum(1000);
  ui->powerFormulaNameEdit->setText(AFormulaSelector::noFormula);
  ui->shumSlider->setMinimum(0);
  ui->shumSlider->setMaximum(10000);
  ui->shumFormulaEdit->setText(AFormulaSelector::noFormula);
  ui->startTimeSlider->setMinimum(0);
  ui->startTimeSlider->setMaximum(10000);
  ui->startTimeFormulaEdit->setText(AFormulaSelector::noFormula);

  //делаем валидатор для чисел с плавающей точкой
  QDoubleValidator *validator = new QDoubleValidator(this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  validator->setLocale(QLocale(QLocale::English));//в английской локали (.) является разделителем
  ui->datchikFrecuencyEdit->setValidator(validator);

  setDatchikFrecuency(50);

  //создаём и настраиваем генераторы значений
  m_powerValueGiver = new AListFormulasValueGiver(&m_powerFormula,frecuency(),ui->autoRepeateBox->isChecked());
  connect(m_powerValueGiver,&AAbstractValueGiver::nextValue,this,&AUsd::onPowerNewValueReady);
  connect(m_powerValueGiver,&AAbstractValueGiver::stoped,this,&AUsd::onAnyValueGiverStoped);
  m_timeValueGiver = new AListFormulasValueGiver(&m_timeFormula,frecuency(),ui->autoRepeateBox->isChecked());
  connect(m_timeValueGiver,&AAbstractValueGiver::nextValue,this,&AUsd::onTimeNewValueReady);
  connect(m_timeValueGiver,&AAbstractValueGiver::stoped,this,&AUsd::onAnyValueGiverStoped);
  m_shumValueGiver = new AListFormulasValueGiver(&m_shumFormula,frecuency(),ui->autoRepeateBox->isChecked());
  connect(m_shumValueGiver,&AAbstractValueGiver::nextValue,this,&AUsd::onShumNewValueReady);
  connect(m_shumValueGiver,&AAbstractValueGiver::stoped,this,&AUsd::onAnyValueGiverStoped);
}

AUsd::~AUsd()
{
  delete ui;
}

void AUsd::addToLog(QString logString)
{
  ui->logEdit->append(logString);
}

void AUsd::clearLog()
{
  ui->logEdit->clear();
}

//void AUsd::on_setFormulaButton_clicked()
//{

//}
qreal AUsd::frecuency() const
{
  return m_frecuency;
}

void AUsd::setFrecuency(const qreal &frecuency)
{
  m_frecuency = frecuency;
}

AChartSeries *AUsd::series()
{
  return m_series;
}

AChart *AUsd::chart()
{
  return ui->chart;
}

qreal AUsd::zatoohanie()
{
  return m_zatuhanie;
}

void AUsd::setZatoohanie(qreal value)
{
  m_zatuhanie = value;
  qreal koef = maxzatoohanie() / (qreal)ui->powerSlider->maximum();
  ui->powerSlider->setValue(value / koef);
  ui->powerEdit->setText(QString::number(m_zatuhanie,'f',3));
}

qreal AUsd::zadergka()
{
  return m_zadergka;
}

void AUsd::setZadergka(qreal value)
{
  m_zadergka = value;
  qreal koef = maxVremyaProhogdeniya() / (qreal)ui->startTimeSlider->maximum();
  ui->startTimeSlider->setValue(value / koef);
  ui->startTimeEdit->setText(QString::number(m_zadergka,'f',3));
}

qreal AUsd::shum()
{
  return m_shum;
}

void AUsd::setShum(qreal value)
{
  m_shum = value;
  qreal koef = maxshum() / (qreal)ui->shumSlider->maximum();
  ui->shumSlider->setValue(value / koef);
  ui->shumEdit->setText(QString::number(m_shum,'f',3));
}

QString AUsd::mainSettingsPath()
{
  QString myName = tr("УСД60NImitator");
#ifdef Q_OS_LINUX
  myName = "." + myName;
#endif
  return QDir::homePath() + QDir::separator() + myName;
}

void AUsd::on_settingsButton_clicked()
{
  viewSettings();
}

void AUsd::on_startButton_clicked()
{
  start();
  ui->startButton->setEnabled(false);
  ui->stopButton->setEnabled(true);
}

void AUsd::on_stopButton_clicked()
{
  stop();
  ui->startButton->setEnabled(true);
  ui->stopButton->setEnabled(false);
}

void AUsd::on_powerSlider_sliderMoved(int position)
{
  qreal koef = maxzatoohanie() / (qreal)ui->powerSlider->maximum();
  m_zatuhanie = (qreal)position * koef;
  ui->powerEdit->setText(QString::number(m_zatuhanie,'f',3));
}

void AUsd::on_startTimeSlider_sliderMoved(int position)
{
  qreal koef = maxVremyaProhogdeniya() / (qreal)ui->startTimeSlider->maximum();
  m_zadergka = (qreal)position * koef;
  ui->startTimeEdit->setText(QString::number(m_zadergka,'f',3));
}

void AUsd::on_shumSlider_sliderMoved(int position)
{
  qreal koef = maxshum() / (qreal)ui->shumSlider->maximum();
  m_shum = (qreal)position * koef;
  ui->shumEdit->setText(QString::number(m_shum,'f',3));
}
qreal AUsd::datchikFrecuency() const
{
  return m_datchikFrecuency;
}

void AUsd::setDatchikFrecuency(const qreal &datchikFrecuency)
{
  m_datchikFrecuency = datchikFrecuency;
  ui->datchikFrecuencyEdit->setText(QString::number(m_datchikFrecuency));
}


void AUsd::on_datchikFrecuencyEdit_editingFinished()
{
  setDatchikFrecuency(ui->datchikFrecuencyEdit->text().toDouble());
}

void AUsd::on_powerFormulaButton_clicked()
{
  m_powerFormula = m_formulaSelector->getFormula("power");
  ui->powerFormulaNameEdit->setText(m_powerFormula.name());
}

void AUsd::on_startTimeFormulaButton_clicked()
{
  m_timeFormula = m_formulaSelector->getFormula("time");
  ui->startTimeFormulaEdit->setText(m_timeFormula.name());
}

void AUsd::on_shumFormulaButton_clicked()
{
  m_shumFormula = m_formulaSelector->getFormula("shum");
  ui->shumFormulaEdit->setText(m_shumFormula.name());
}

void AUsd::onPowerNewValueReady(qreal value)
{
  if (!ui->powerSlider->isSliderDown())
    setZatoohanie(value);
}

void AUsd::onTimeNewValueReady(qreal value)
{
  if (!ui->startTimeSlider->isSliderDown())
    setZadergka(value);
}

void AUsd::onShumNewValueReady(qreal value)
{
  if (!ui->shumSlider->isSliderDown())
    setShum(value);
}

void AUsd::onAnyValueGiverStoped()
{
  if ( (!m_powerValueGiver->isActive()) &&
       (!m_timeValueGiver->isActive()) &&
       (!m_shumValueGiver->isActive()) )
    ui->autoReplayButton->setChecked(false);
}

void AUsd::on_autoReplayButton_clicked(bool checked)
{
  if (checked)
    {
      if (ui->powerFormulaNameEdit->text() != AFormulaSelector::noFormula)
        m_powerValueGiver->start();
      if (ui->startTimeFormulaEdit->text() != AFormulaSelector::noFormula)
        m_timeValueGiver->start();
      if (ui->shumFormulaEdit->text() != AFormulaSelector::noFormula)
        m_shumValueGiver->start();
    }
  else
    {
      m_powerValueGiver->stop();
      m_timeValueGiver->stop();
      m_shumValueGiver->stop();
    }
}

void AUsd::on_autoRepeateBox_clicked(bool checked)
{
  m_powerValueGiver->setAutoRepeate(checked);
  m_timeValueGiver->setAutoRepeate(checked);
  m_shumValueGiver->setAutoRepeate(checked);
}
