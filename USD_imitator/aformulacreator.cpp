#include <QScreen>
#include "aformulacreator.h"
#include "ui_aformulacreator.h"

AFormulaCreator::AFormulaCreator(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AFormulaCreator)
{
  m_frecuency = 15;
  m_destroying = false;

  ui->setupUi(this);
  QRect screenRect = QApplication::primaryScreen()->geometry();
  int w = screenRect.width() * 0.8;
  int l = screenRect.width()/2 - w/2;
  setGeometry(l,screenRect.height() * 0.25,w,screenRect.height()/2);
  setLayout(&m_mainLayout);
  m_formulaName = new QLineEdit(this);
  m_formulaName->setAlignment(Qt::AlignHCenter);
  m_mainLayout.addWidget(m_formulaName);
  m_formulaNameLabel = new QLabel(tr("Наименование"),this);
  m_formulaNameLabel->setAlignment(Qt::AlignHCenter);
  m_mainLayout.addWidget(m_formulaNameLabel);
  m_chart = new AChart(this);
  m_mainLayout.addWidget(m_chart);
  m_currformulaEdit = new QTextEdit(this);
  m_currformulaEdit->setMaximumHeight(200);
  m_mainLayout.addWidget(m_currformulaEdit);
  m_series = new AChartSeries(this,"График");
  m_chart->addSeries(m_series);
  m_mainLayout.addLayout(&m_intervalsLayout);
  m_okButton = new QPushButton(tr("Применить"));
  m_okButton->setEnabled(false);
  m_createGraphButton = new QPushButton(tr("Построить график"));
  m_mainButtonLayout.addWidget(m_okButton);
  m_mainButtonLayout.addWidget(m_createGraphButton);
  m_mainLayout.addLayout(&m_mainButtonLayout);
  m_currentIntervalShower = NULL;

  connect(m_formulaName,&QLineEdit::textChanged,this,&AFormulaCreator::onNameChanged);
  connect(m_okButton,&QPushButton::clicked,this,&AFormulaCreator::onOkButtonClicked);
  connect(m_currformulaEdit,&QTextEdit::textChanged,this,&AFormulaCreator::onCurrformulaEditTextChanged);
  connect(m_createGraphButton,&QPushButton::clicked,this,&AFormulaCreator::onCreateGraphButtonClicked);
//  connect(m_currformulaEdit,&QTextEdit::,this,&AFormulaCreator::onCurrformulaEditEditFinished);

  //создаём визуальное окружение
  reBuild();
}

AFormulaCreator::~AFormulaCreator()
{
  m_destroying = true;
  delete ui;
}

AIntervalContainer AFormulaCreator::intervals()
{
  return m_intervals;
}

void AFormulaCreator::setIntervals(AIntervalContainer intervals)
{
  m_intervals = intervals;
  m_formulaName->setText(intervals.name());
  reBuild();
  updateTrend();
}

void AFormulaCreator::onInserterPressed(AInserter *inserter)
{
  //если вставляем в начала списка
  qreal startTime = 0;
  if ( (inserter->num() == 0) )
    {
      startTime = 0;
    }
  //если вставляем внутрь списка
  else
    {
      startTime = m_intervals[inserter->num() - 1].end();
    }
  AInterval interval(startTime);
  m_intervals.insert(inserter->num(),interval);
  reBuild();
}

void AFormulaCreator::onIntervalRemoved(int numInterval)
{
  if (m_destroying) return;
  m_intervals.removeAt(numInterval);
  //изменяем стартовое время для всех последующих интервалов
  for (int i = numInterval - 1;i < intervals().count() - 1;i++)
    {
      qreal startTime;
      if (i >= 0)
        startTime = m_intervals[i].end();
      else
        startTime = 0;
      m_intervals[i + 1].setStartTime(startTime);
    }
  reBuild();
  updateTrend();
  intervalShowersUpdate();
}

void AFormulaCreator::onIntervalTimeEditFinished(int numInterval)
{
  //изменяем стартовое время для всех последующих интервалов
  for (int i = numInterval;i < intervals().count() - 1;i++)
    {
      qreal startTime = m_intervals[i].end();
      m_intervals[i + 1].setStartTime(startTime);
    }
  intervalShowersUpdate();
//  updateTrend();
}

void AFormulaCreator::onIntervalFormulaEditFinished(int numInterval)
{
  numInterval = numInterval;
//  updateTrend();
}

void AFormulaCreator::onIntervalSelected(AIntervalShower *intervalShower)
{
  m_currentIntervalShower = intervalShower;
  QString formula = m_currentIntervalShower->interval()->formula();
  m_currformulaEdit->setText(formula);
}

void AFormulaCreator::onIntervalFormulaChanged(QString formula)
{
  if (formula != m_currformulaEdit->toPlainText())
    m_currformulaEdit->setText(formula);
}

void AFormulaCreator::onCurrformulaEditTextChanged()
{
  QString formula = m_currformulaEdit->toPlainText();
  if (m_currentIntervalShower != NULL)
    {
      int numInterval = m_currentIntervalShower->num();
      if (m_intervals[numInterval].formula() != formula)
        {
          m_intervals[numInterval].setFormula(formula);
          m_currentIntervalShower->updateValues();
        }
    }
}

void AFormulaCreator::updateTrend()
{
  m_createGraphButton->setText(tr("Идёт построение графика"));
  m_createGraphButton->repaint();
  m_series->clear();
  //рисуем график
  qreal delta = 1/m_frecuency;
  for (int num = 0;num < m_intervals.count();num++)
    {
      for (qreal time = 0;time < m_intervals[num].length();time += delta)
        {
          m_engine.globalObject().setProperty("t",time);
          qreal y = m_engine.evaluate(m_intervals[num].formula()).toNumber();
          qreal x = m_intervals[num].begin() + time;
          m_series->addPoint(x,y);
        }
    }
  m_chart->update();
  m_createGraphButton->setText(tr("Построить график"));
}

void AFormulaCreator::onNameChanged(QString name)
{
  if (name != "")
    m_okButton->setEnabled(true);
  else
    m_okButton->setEnabled(false);
  m_intervals.setName(name);
}

void AFormulaCreator::onOkButtonClicked()
{
  emit changed();
  close();
}

void AFormulaCreator::onCreateGraphButtonClicked()
{
  updateTrend();
}

void AFormulaCreator::intervalShowersUpdate()
{
  for (int i = 0;i < m_intervalShowers.count();i++)
    m_intervalShowers[i]->updateValues();
}
qreal AFormulaCreator::frecuency() const
{
  return m_frecuency;
}

void AFormulaCreator::setFrecuency(const qreal &frecuency)
{
  m_frecuency = frecuency;
}

QByteArray AFormulaCreator::data()
{
  QByteArray data;
  QDataStream in(&data,QIODevice::WriteOnly);

  return data;
}

void AFormulaCreator::clear()
{
  m_intervals.clear();
  m_formulaName->setText("");
  reBuild();
  updateTrend();
}


void AFormulaCreator::reBuild()
{
  // удаляем всё визуальное
  if (m_scrollArea) m_scrollArea->deleteLater();
  m_intervalShowers.clear();

  //создаём визуальное окружение
  m_scrollArea = new QScrollArea(this);
  QSizePolicy areaSize = m_scrollArea->sizePolicy();
  areaSize.setVerticalPolicy(QSizePolicy::Maximum);
  m_scrollArea->setSizePolicy(areaSize);
  m_intervalsLayout.addWidget(m_scrollArea);
  QWidget *widget = new QWidget(m_scrollArea);
  QHBoxLayout *widgetlayout = new QHBoxLayout(widget);
  widget->setLayout(widgetlayout);

  AInserter *inserter = new AInserter(0,m_scrollArea);
  connect(inserter,&AInserter::mousePressed,this,&AFormulaCreator::onInserterPressed);
  widgetlayout->addWidget(inserter);

  for (int i = 0; i < m_intervals.count();i++)
    {
      AIntervalShower *intervalShower = new AIntervalShower(&m_intervals[i],i,m_scrollArea);
      m_intervalShowers << intervalShower;
      widgetlayout->addWidget(intervalShower);
      AInserter *inserter = new AInserter(i+1,m_scrollArea);
      widgetlayout->addWidget(inserter);
      connect(inserter,&AInserter::mousePressed,this,&AFormulaCreator::onInserterPressed);
      connect(intervalShower,&AIntervalShower::timeIntervalEditFinished,this,&AFormulaCreator::onIntervalTimeEditFinished);
      connect(intervalShower,&AIntervalShower::formulaEditFinished,this,&AFormulaCreator::onIntervalFormulaEditFinished);
      connect(intervalShower,&AIntervalShower::removed,this,&AFormulaCreator::onIntervalRemoved);
      connect(intervalShower,&AIntervalShower::selected,this,&AFormulaCreator::onIntervalSelected);
      connect(intervalShower,&AIntervalShower::formulaChanged,this,&AFormulaCreator::onIntervalFormulaChanged);
    }

  m_scrollArea->setWidget(widget);
}
