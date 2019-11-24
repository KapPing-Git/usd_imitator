#include "ausd60Settings.h"
#include <QMessageBox>
#include <QIntValidator>
#include <math.h>
#include <QApplication>
#include <QDir>
#include <QTextCodec>

AUSD60_Settings::AUSD60_Settings(QWidget *parent) : QWidget(parent)
{
  //коеффициенты соответствия
  rangeKof = 0.1;
  delayKof = 0.1;

  formCreate();
  setGeometry(200,100,700,800);
  //загружаем из файла настройки для соединения с прибором
  //Если сохранённых настроек нет, то берём стандартные

  if (!loadUsd60Settings()) setStandartSettings();
  reedUsdSettings();
}

AUSD60_Settings::~AUSD60_Settings()
{
//  sendUsd60StopCommand();
}

void AUSD60_Settings::setParams(AUsdControlParams usdParams)
{
  m_usdControlParams = usdParams;
  verifyUsdSettings();
  reedUsdSettings();
}

AUSD60_Settings::AUsdControlParams AUSD60_Settings::usdParams()
{
  return m_usdControlParams;
}

//блок методов с помощью которых можно задать настройки прибора
void AUSD60_Settings::setGain(float digit)
{
  if ( (digit < 0) or (digit > 90))
    {
      emit errorValueInputed(tr("Значение усиления %1 выходит за пределы допустимых значений 0 - 90").arg(digit));
      digit = qBound((float)0,digit,(float)90);
    }
  m_usdControlParams.gain=digit*2;
}

float AUSD60_Settings::gain()
{
  return((qreal)m_usdControlParams.gain/2);
}

void AUSD60_Settings::setRange(float digit)
{
  m_usdControlParams.range=ceil(digit / rangeKof);
}

float AUSD60_Settings::range()
{
  return((qreal)m_usdControlParams.range*rangeKof);
}

void AUSD60_Settings::setDelay(float digit)
{
  m_usdControlParams.delay=ceil(digit / delayKof);
}

float AUSD60_Settings::delay()
{
  return((qreal)m_usdControlParams.delay*delayKof);
}

void AUSD60_Settings::setReject(float digit)
{
  m_usdControlParams.reject=digit;
}

float AUSD60_Settings::reject()
{
  return((qreal)m_usdControlParams.reject);
}

void AUSD60_Settings::setA_thresh(float digit)
{
  m_usdControlParams.a_thresh=digit;
}

float AUSD60_Settings::a_thresh()
{
  return((qreal)m_usdControlParams.a_thresh);
}

void AUSD60_Settings::setA_start(float digit)
{
  m_usdControlParams.a_start=ceil(digit / rangeKof);
}

float AUSD60_Settings::a_start()
{
  return((qreal)m_usdControlParams.a_start*rangeKof);
}

void AUSD60_Settings::setA_width(float digit)
{
  m_usdControlParams.a_width=ceil(digit / rangeKof);
}

float AUSD60_Settings::a_width()
{
  return((qreal)m_usdControlParams.a_width*rangeKof);
}

void AUSD60_Settings::setFrecuency(float digit)
{
  m_usdControlParams.pulse_width=10000/digit-1;
}

float AUSD60_Settings::frecuency()
{
  return((qreal)10000/(m_usdControlParams.pulse_width+1));
}

float AUSD60_Settings::pulseWitdh()
{
  return (qreal)(m_usdControlParams.pulse_width+1) / 10000;
}

void AUSD60_Settings::setPulse_count(int digit)
{
  m_usdControlParams.pulse_count=digit;
}

int AUSD60_Settings::pulse_count()
{
  return(m_usdControlParams.pulse_count);
}

void AUSD60_Settings::setTimer_freq(int digit)
{
  m_usdControlParams.timer_freq=digit;
}

int AUSD60_Settings::timer_freq()
{
  return(m_usdControlParams.timer_freq);
}

void AUSD60_Settings::setPolosa(AUSD60_Settings::polosaEnum polosa)
{
  m_usdControlParams.polosa=polosa;
}

AUSD60_Settings::polosaEnum AUSD60_Settings::polosa()
{
  return((polosaEnum)m_usdControlParams.polosa);
}

void AUSD60_Settings::setDetector(AUSD60_Settings::detectorEnum detector)
{
  m_usdControlParams.detector=detector;
}

AUSD60_Settings::detectorEnum AUSD60_Settings::detector()
{
  return((detectorEnum)m_usdControlParams.detector);
}

void AUSD60_Settings::setVrch_points(int digit)
{
  digit = qBound(0,digit,VRCH_POINT_COUNT);
  m_usdControlParams.vrch_points=digit;
}

int AUSD60_Settings::vrch_points()
{
  return(m_usdControlParams.vrch_points);
}

void AUSD60_Settings::setVrchTimes(int numTime, float time)
{
  if ((numTime < 0) || (numTime > VRCH_POINT_COUNT)) return;
  time = qBound((float)0,time,(float)6000);
  m_usdControlParams.vrch_times[numTime]= round(time * 10);
}

float AUSD60_Settings::vrchTimes(int numTime)
{
  return((float)m_usdControlParams.vrch_times[numTime] / 10);
}

void AUSD60_Settings::setVrchGains(int numTime, float gain)
{
  if ((numTime < 0) || (numTime > VRCH_POINT_COUNT)) return;
  gain = qBound((float)0,gain,(float)90);
  m_usdControlParams.vrch_gains[numTime]=round(gain * 2);
}

float AUSD60_Settings::vrchGains(int numTime)
{
  return((float)m_usdControlParams.vrch_gains[numTime] / 2);
}

void AUSD60_Settings::setVrch_mode(int vrch)
{
  m_usdControlParams.vrch_mode=vrch;
}

int AUSD60_Settings::vrch_mode()
{
  return(m_usdControlParams.vrch_mode);
}
//-------------------------------------------------------------------------

void AUSD60_Settings::onUserChangeSettings()
{
  if (m_includeVrchBox->currentIndex() == 0)
    {
      m_vrchPointsEdit->setEnabled(false);
      m_vrchTable->setEnabled(false);
    }
  else
    {
      m_vrchPointsEdit->setEnabled(true);
      m_vrchTable->setEnabled(true);
    }

  writeUsdSettings();
  verifyUsdSettings();
  reedUsdSettings();
  emit usdSettingsChenged();
}

void AUSD60_Settings::onChangeAStartEdit(QString text)
{
  m_aWidthEdit->lineEdit->setValidator(new QIntValidator(0,60000-text.toInt(),this));
}

void AUSD60_Settings::onVrchTableChanged(int row, int col)
{
  if (col == 0)
    {
      QTableWidgetItem *time = m_vrchTable->item(row,col);
      setVrchTimes(row,time->text().toFloat());
      time->setText(QString::number(vrchTimes(row)));
    }
  if (col == 1)
    {
      QTableWidgetItem *gain = m_vrchTable->item(row,col);
      setVrchGains(row,gain->text().toFloat());
      gain->setText(QString::number(vrchGains(row)));
    }
  emit usdSettingsChenged();
}

void AUSD60_Settings::reedUsdSettings()
{
  m_ganeEdit->lineEdit->setText(QString::number(gain()));
  m_rangeEdit->lineEdit->setText(QString::number(range()));
  m_delayEdit->lineEdit->setText(QString::number(delay()));
  m_rejectEdit->lineEdit->setText(QString::number(reject()));
  m_aThreshEdit->lineEdit->setText(QString::number(a_thresh()));
  m_aStartEdit->lineEdit->setText(QString::number(a_start()));
  m_aWidthEdit->lineEdit->setText(QString::number(a_width()));
  m_frecuencyEdit->lineEdit->setText(QString::number(frecuency()));
  m_pulseCountEdit->lineEdit->setText(QString::number(pulse_count()));
  m_timerFreqEdit->lineEdit->setText(QString::number(timer_freq()));
  m_polosaBox->setCurrentIndex(polosa());
  m_detectorBox->setCurrentIndex(detector());

  m_includeVrchBox->setCurrentIndex(vrch_mode());
  m_vrchPointsEdit->lineEdit->setText(QString::number(vrch_points()));
  for (int i = 0;i < vrch_points();i++)
    {
      if (m_vrchTable->item(i,0) == NULL) m_vrchTable->setItem(i,0,new QTableWidgetItem());
      if (m_vrchTable->item(i,1) == NULL) m_vrchTable->setItem(i,1,new QTableWidgetItem());
      m_vrchTable->item(i,0)->setText(QString::number(vrchTimes(i)));
      m_vrchTable->item(i,1)->setText(QString::number(vrchGains(i)));
    }
}

void AUSD60_Settings::writeUsdSettings()
{
  setGain(m_ganeEdit->lineEdit->text().toFloat());
  setRange(m_rangeEdit->lineEdit->text().toFloat());
  setDelay(m_delayEdit->lineEdit->text().toFloat());
  setReject(m_rejectEdit->lineEdit->text().toFloat());
  setA_thresh(m_aThreshEdit->lineEdit->text().toFloat());
  setA_start(m_aStartEdit->lineEdit->text().toFloat());
  setA_width(m_aWidthEdit->lineEdit->text().toFloat());
  setFrecuency(m_frecuencyEdit->lineEdit->text().toFloat());
  setPulse_count(m_pulseCountEdit->lineEdit->text().toInt());
  setTimer_freq(m_timerFreqEdit->lineEdit->text().toInt());
  setPolosa((polosaEnum)m_polosaBox->currentIndex());
  setDetector((detectorEnum)m_detectorBox->currentIndex());

  setVrch_mode(m_includeVrchBox->currentIndex());
  setVrch_points(m_vrchPointsEdit->lineEdit->text().toInt());
  m_vrchTable->setRowCount(vrch_points());
}

void AUSD60_Settings::formCreate()
{
  m_settingsLayout = new QVBoxLayout();
  m_ganeEdit = addDoubleValidateNameLineEditToLayout(tr("Усиление"),m_settingsLayout,0,90);
  m_rangeEdit = addDoubleValidateNameLineEditToLayout(tr("Развертка экрана"),m_settingsLayout,0,6000);
  m_delayEdit = addDoubleValidateNameLineEditToLayout(tr("Задержка экран"),m_settingsLayout,0,6000);
  m_rejectEdit = addDoubleValidateNameLineEditToLayout(tr("Отсчека"),m_settingsLayout,0,80);
  m_aThreshEdit = addDoubleValidateNameLineEditToLayout(tr("Уровень порога зоны"),m_settingsLayout,-95,95);
  m_aStartEdit = addDoubleValidateNameLineEditToLayout(tr("Начало зоны контроля"),m_settingsLayout,0,6000);
  m_aWidthEdit = addDoubleValidateNameLineEditToLayout(tr("Ширина зоны контроля "),m_settingsLayout,0,6000);
    //пределы для aWidthEdit зависят от aStartEdit
  connect(m_aStartEdit->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(onChangeAStartEdit(QString)));
  m_frecuencyEdit=addDoubleValidateNameLineEditToLayout(tr("Частота генератора, кГц"),m_settingsLayout,20,2500);

  m_pulseCountEdit=addIntValidateNameLineEditToLayout(tr("Число периодов импульса возбуждения"),m_settingsLayout,0,10);
  m_timerFreqEdit=addIntValidateNameLineEditToLayout(tr("Частота посылок (измерений), Гц"),m_settingsLayout,10,200);

  m_polosaLabel=new QLabel(tr("Выбор аналогового фильтра"),this);
  m_polosaBox=new QComboBox(this);
  m_polosaBox->addItem(tr("400 .. 2500 кГц"));
  m_polosaBox->addItem(tr("20 .. 100 кГц"));
  m_polosaBox->addItem(tr("20 .. 300 кГц"));
  m_polosaBox->addItem(tr("200 .. 1250 кГц"));
  m_polosaLayout = new QHBoxLayout();
  m_polosaLayout->addWidget(m_polosaLabel);
  m_polosaLayout->addWidget(m_polosaBox);
  connect(m_polosaBox,&QComboBox::currentTextChanged,this,&AUSD60_Settings::onUserChangeSettings);
  m_settingsLayout->addLayout(m_polosaLayout);

  m_detectorLabel=new QLabel(tr("Выбор детекирования"),this);
  m_detectorBox=new QComboBox();
  m_detectorBox->addItem(tr("Радиосигнал"));
  m_detectorBox->addItem(tr("Плюс"));
  m_detectorBox->addItem(tr("Минус"));
  m_detectorBox->addItem(tr("Полное"));
  m_detectorLayout = new QHBoxLayout();
  m_detectorLayout->addWidget(m_detectorLabel);
  m_detectorLayout->addWidget(m_detectorBox);
  connect(m_detectorBox,&QComboBox::currentTextChanged,this,&AUSD60_Settings::onUserChangeSettings);
  m_settingsLayout->addLayout(m_detectorLayout);

  //---------
  //врч
  m_includeVrchLabel = new QLabel(tr("ВРЧ усиление"),this);
  m_includeVrchBox = new QComboBox();
  m_includeVrchBox->addItem(tr("Выключить"));
  m_includeVrchBox->addItem(tr("Включить"));
  m_includeVrchLayout = new QHBoxLayout();
  m_includeVrchLayout->addWidget(m_includeVrchLabel);
  m_includeVrchLayout->addWidget(m_includeVrchBox);
  connect(m_includeVrchBox,&QComboBox::currentTextChanged,this,&AUSD60_Settings::onUserChangeSettings);
  m_settingsLayout->addLayout(m_includeVrchLayout);

  m_vrchPointsEdit = addIntValidateNameLineEditToLayout(tr("Кол-во точек ВРЧ"),m_settingsLayout,0,32);
  m_vrchPointsEdit->setEnabled(false);
  m_vrchPointsEdit->lineEdit->setText(QString::number(VRCH_POINT_COUNT));
  
  m_vrchTable = new QTableWidget(0,2,this);
  m_vrchTable->setEnabled(false);
  m_vrchTable->setColumnCount(2);
  m_vrchTable->setRowCount(VRCH_POINT_COUNT);
  //задаём названия полей
  QStringList vrchHHeaders;
  vrchHHeaders << "ВРЧ время" << "ВРЧ усиление";
  m_vrchTable->setHorizontalHeaderLabels(vrchHHeaders);
  QStringList vrchVHeaders;
  for (int i = 0;i < VRCH_POINT_COUNT;i++)
    vrchVHeaders << QString::number(i);
  m_vrchTable->setVerticalHeaderLabels(vrchVHeaders);
  connect(m_vrchTable,&QTableWidget::cellChanged,this,&AUSD60_Settings::onVrchTableChanged);

  m_mainLayout = new QHBoxLayout();
  m_mainLayout->addLayout(m_settingsLayout);
  m_mainLayout->addWidget(m_vrchTable);

  setLayout(m_mainLayout);
}

ANamedLineEdit *AUSD60_Settings::addIntValidateNameLineEditToLayout(QString name, QLayout *layout, int bottom, int top)
{
  ANamedLineEdit *edit=new ANamedLineEdit(this,name+" "+QString::number(bottom)+"-"+QString::number(top));
  edit->lineEdit->setValidator(new QIntValidator(bottom,top,this));
  layout->addWidget(edit);
  connect(edit->lineEdit,&QLineEdit::editingFinished,this,&AUSD60_Settings::onUserChangeSettings);
  return(edit);
}

ANamedLineEdit *AUSD60_Settings::addDoubleValidateNameLineEditToLayout(QString name, QLayout *layout, double bottom, double top)
{
#define DECIMALS 5
  ANamedLineEdit *edit = new ANamedLineEdit(this,name+" "+QString::number(bottom)+"-"+QString::number(top));
  QDoubleValidator *validator = new QDoubleValidator(bottom,top,DECIMALS,this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  validator->setLocale(QLocale(QLocale::English));//в английской локали (.) является разделителем
  edit->lineEdit->setValidator(validator);
  layout->addWidget(edit);
  connect(edit->lineEdit,&QLineEdit::editingFinished,this,&AUSD60_Settings::onUserChangeSettings);
  return(edit);
}

void AUSD60_Settings::closeEvent(QCloseEvent * event)
{
  event = event;
  saveSettings();
}

//проверяем настройки прибора, если они выходят за границы разрешённых, то меняем их
void AUSD60_Settings::verifyUsdSettings()
{
#define test_min(value, min) if (value < min) value = min
#define test_max(value, max) if (value > max) value = max
#define test(value, min, max) test_min(value, min); test_max(value, max);

  test_max(m_usdControlParams.gain, 220);

  test(m_usdControlParams.range, 160, 60000);
  test_max(m_usdControlParams.delay, 60000);
  test_max(m_usdControlParams.reject, 80);

  test_max(m_usdControlParams.a_start, 60000);
  test_max(m_usdControlParams.a_width, 60000 - m_usdControlParams.a_start);
  test(m_usdControlParams.a_thresh, -95, 95);

  test(m_usdControlParams.pulse_width, 3, 499);
  test_max(m_usdControlParams.pulse_count, 31);

  test(m_usdControlParams.timer_freq, 10, 200);

  test_max(m_usdControlParams.polosa, 3);
  test_max(m_usdControlParams.detector, 3);
  for (int i = 0;i < VRCH_POINT_COUNT;i++)
    {
      test_max(m_usdControlParams.vrch_times[i],60000);
      test_max(m_usdControlParams.vrch_gains[i],220);
    }

  m_usdControlParams.gzi_hi = 1; // зондирующий всегда 200 В
}

bool AUSD60_Settings::loadUsd60Settings()
{
  QFile usd60SettingsFile(setingsFileName());
  if (usd60SettingsFile.open(QIODevice::ReadOnly))
    {
      QDataStream out(&usd60SettingsFile);
      QByteArray byteArray;
      out >> byteArray;
      m_usdControlParams = *(AUsdControlParams*)byteArray.data();
      usd60SettingsFile.close();
      return(true);
    }
  else return(false);
}

void AUSD60_Settings::saveSettings()
{
  //если не существует нужных директорий, то создаём их
  QDir dir=QFileInfo(setingsFileName()).dir();
  if (!dir.exists()) dir.mkpath(dir.dirName());
  QFile usd60SettingsFile(setingsFileName());
  if (usd60SettingsFile.open(QIODevice::WriteOnly))
    {
      QDataStream in(&usd60SettingsFile);
      in << QByteArray((char*)&m_usdControlParams,sizeof(m_usdControlParams));
      usd60SettingsFile.close();
    }
}

QString AUSD60_Settings::setingsFileName()
{
  QString myName = tr("УСД60NImitator");
#ifdef Q_OS_LINUX
  myName = "." + myName;
#endif
  return QDir::homePath() + QDir::separator() + myName + QDir::separator() + "USD60_Settings";
//  return(QApplication::applicationDirPath()+QDir::separator()+"USD60"+QDir::separator()+"USD60_Settings");
}

void AUSD60_Settings::setStandartSettings()
{
  memset(&m_usdControlParams, 0, sizeof(m_usdControlParams));
  m_usdControlParams.range = 20000;
  m_usdControlParams.a_start = 7000;
  m_usdControlParams.a_width = 3000;
  m_usdControlParams.a_thresh = 50;
  m_usdControlParams.pulse_width = 199;
  m_usdControlParams.pulse_count = 6;
  m_usdControlParams.timer_freq = 15;
  m_usdControlParams.polosa = polosaEnum_20_100;
}

