#include "aintervalshower.h"
#include "ui_aintervalshower.h"

AIntervalShower::AIntervalShower(AInterval *interval, int num, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::AIntervalShower)
{
  ui->setupUi(this);
  m_interval = interval;
  m_numAtContainer = num;
  updateValues();
  m_delAction = m_menu.addAction("Удалить");
  connect(m_delAction,&QAction::triggered,this,&AIntervalShower::onDelAction);
  ui->formulaEdit->installEventFilter(this);
  ui->timeEdit->installEventFilter(this);
}

AIntervalShower::~AIntervalShower()
{
  delete ui;
}

int AIntervalShower::num()
{
  return m_numAtContainer;
}

void AIntervalShower::updateValues()
{
  ui->timeEdit->setText(QString::number(m_interval->length()));
  ui->formulaEdit->setText(m_interval->formula());
  ui->intervalLabel->setText(QString::number(m_interval->begin()) + " - " + QString::number(m_interval->end()));
}

AInterval *AIntervalShower::interval()
{
  return m_interval;
}

void AIntervalShower::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::RightButton)
    {
      m_menu.setGeometry(event->globalPos().x(),event->globalPos().y(),100,20);
      m_menu.show();
    }
}

void AIntervalShower::mousePressEvent(QMouseEvent *event)
{
  event = event;
  selected(this);
}

bool AIntervalShower::eventFilter(QObject *target, QEvent *event)
{
  target = target;
  if (event->type() == QEvent::MouseButtonPress)
    selected(this);
  return false;
}

void AIntervalShower::on_formulaEdit_textChanged(const QString &arg1)
{
  QScriptSyntaxCheckResult::State resState = m_engine.checkSyntax(arg1).state();
  QPalette red = QPalette();
  QPalette yellow = QPalette();
  QPalette black = QPalette();
  red.setColor(QPalette::Text,QColor(Qt::red));
  yellow.setColor(QPalette::Text,QColor(Qt::yellow));
  black.setColor(QPalette::Text,QColor(Qt::black));
  if (resState == QScriptSyntaxCheckResult::Error)
      ui->formulaEdit->setPalette(red);
  if (resState == QScriptSyntaxCheckResult::Intermediate)
      ui->formulaEdit->setPalette(yellow);
  if (resState == QScriptSyntaxCheckResult::Valid)
    {
      ui->formulaEdit->setPalette(black);
    }
  m_interval->setFormula(arg1);
  emit formulaChanged(arg1);
}

void AIntervalShower::on_timeEdit_editingFinished()
{
  m_interval->setLength(ui->timeEdit->text().toDouble());
  ui->intervalLabel->setText(QString::number(m_interval->begin()) + " - " + QString::number(m_interval->end()));
  emit timeIntervalEditFinished(m_numAtContainer);
}

void AIntervalShower::onDelAction()
{
  emit removed(m_numAtContainer);
  deleteLater();
}

void AIntervalShower::on_delButton_clicked()
{
  onDelAction();
}

void AIntervalShower::on_formulaEdit_editingFinished()

{
  m_interval->setFormula(ui->formulaEdit->text());
  emit formulaEditFinished(m_numAtContainer);
}
