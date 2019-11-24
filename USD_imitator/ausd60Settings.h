#ifndef AUSD60_SETTINGS_H
#define AUSD60_SETTINGS_H
#include <QWidget>
#include <QHostAddress>
#include<QTcpSocket>
#include <QUdpSocket>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include "anamedlineedit.h"

class AUSD60_Settings : public QWidget
{
  Q_OBJECT

#define VRCH_POINT_COUNT 32

public:  
  explicit AUSD60_Settings(QWidget *parent=0);
  ~AUSD60_Settings();

  enum polosaEnum{polosaEnum_400_2500=0,
                  polosaEnum_20_100=1,
                  polosaEnum_20_300=2,
                  polosaEnum_200_1250=3};
  enum detectorEnum{detectorEnum_radio=0,
                    detectorEnum_plus=1,
                    detectorEnum_minus=2,
                    detectorEnum_full};

  //структура данных настройки USD60
  #pragma pack(push, 2) // выравнивание всех срутктур данных - 16 бит (слово)
  typedef struct {                     // Параметры работы канала в формате УЗ канала
                                       // Все временные параметры: развертка, задержка, зоны
                                       // задаютя с дискретностью 0,1 мкс

           quint16 gain;               // усиление - 0..180 с дискретностью 0.5 дБ

           quint32 range;                  // развертка экрана от 160 до 60000
           quint32 delay;                  // задержка экрана от 0 до 60000
           quint16 reject;             // отсчека от 0 до 80 % высоты экрана

           qint16 a_thresh;           // уровень порога зоны 0..95 % высоты экрана
                                       // при радиосигнале -95..95 % высоты экрана
           quint32 a_start;                // начало зоны контроля от 0 до 60000
           quint32 a_width;                // ширина зоны контроля от 0 до 60000-a_start

           quint16 pulse_width;        // ширина полупериода импульса возбуждения
                                       // будет равна 0.05 мкс * (pulse_width + 1)
           quint16 pulse_count;        // число периодов импульса возбуждения от 0 до 31

           quint16 gzi_hi;             // выбор амплитуды импульса возбуждени 50/200 В

           quint16 timer_freq;         // частота посылок (измерений), Гц,  с которой
                                       // дефектоскоп будет работать и присылать по
                                       // UDP  результаты в ПК

           quint16 polosa;             // выбор аналогового фильтра
                                       // 0 - 400 .. 2500 кГц
                                       // 1 - 20 .. 100 кГц
                                       // 2 - 20 .. 300 кГц
                                       // 3 - 200 .. 1250 кГц

           quint16 detector;           // выбор детекирования:
                                       // 0 - радиосигнал
                                       // 1 - плюс
                                       // 2 - минсу
                                       // 3 - полное

           quint16 vrch_points;        // число точек ВРЧ
           quint16 vrch_times[VRCH_POINT_COUNT];     // положение точек ВРЧ, от 0 до 6000, с дискретностью 1 мкс
           quint16 vrch_gains[VRCH_POINT_COUNT];     // усиление в точках ВРЧ, от -180 до 180, с дискретностью 0.5 дБ
           quint16 vrch_mode;          // включение/выключение ВРЧ

          } AUsdControlParams;
  #pragma pack(pop)
  //------------------

public slots:
  void setParams(AUsdControlParams usdParams);
  AUsdControlParams usdParams();
//блок методов с помощью которых можно задать настройки прибора
  void setGain(float digit);               // усиление - 0..180 с дискретностью 0.5 дБ
  void setRange(float digit);              // развертка экрана от 160 до 60000
  void setDelay(float digit);              // задержка экрана от 0 до 60000
  void setReject(float digit);             // отсчека от 0 до 80 % высоты экрана
  void setA_thresh(float digit);           // уровень порога зоны 0..95 % высоты экрана
                                         // при радиосигнале -95..95 % высоты экрана
  void setA_start(float digit);            // начало зоны контроля от 0 до 60000
  void setA_width(float digit);            // ширина зоны контроля от 0 до 60000-a_start

  void setFrecuency(float digit);         // Частота импульса возбуждения

  void setPulse_count(int digit);         // число периодов импульса возбуждения от 0 до 31

//  void setGzi_hi(int digit);              // выбор амплитуды импульса возбуждени 50/200 В

  void setTimer_freq(int digit);          // частота посылок (измерений), Гц,  с которой
                                          // дефектоскоп будет работать и присылать по
                                          // UDP  результаты в ПК

  void setPolosa(polosaEnum polosa);             // выбор аналогового фильтра

  void setDetector(detectorEnum detector);           // выбор детекирования:

  void setVrch_points(int digit);        // число точек ВРЧ
  void setVrchTimes(int numTime, float time);     // положение точек ВРЧ, от 0 до 6000, с дискретностью 1 мкс
  void setVrchGains(int numTime,float gain);      // усиление в точках ВРЧ, от -180 до 180, с дискретностью 0.5 дБ
  void setVrch_mode(int vrch);          // включение/выключение ВРЧ
//------------------------

public:
  //блок методов позволяющих читать настройки прибора
  float gain();
  float range();              // развертка экрана от 160 до 60000
  float delay();              // задержка экрана от 0 до 60000
  float reject();             // отсчека от 0 до 80 % высоты экрана
  float a_thresh();           // уровень порога зоны 0..95 % высоты экрана                                       // при радиосигнале -95..95 % высоты экрана
  float a_start();            // начало зоны контроля от 0 до 60000
  float a_width();            // ширина зоны контроля от 0 до 60000-a_start
  float frecuency();         // Частота импульса возбуждения
  float pulseWitdh();         // Частота импульса возбуждения
  int pulse_count();         // число периодов импульса возбуждения от 0 до 31
  int timer_freq();          // частота посылок (измерений), Гц,  с которой
                                          // дефектоскоп будет работать и присылать по
                                          // UDP  результаты в ПК
  polosaEnum polosa();             // выбор аналогового фильтра
  detectorEnum detector();           // выбор детекирования:
  int vrch_points();        // число точек ВРЧ
  float vrchTimes(int numTime);     // положение точек ВРЧ, от 0 до 6000, с дискретностью 1 мкс
  float vrchGains(int numTime);      // усиление в точках ВРЧ, от -180 до 180, с дискретностью 0.5 дБ
  int vrch_mode();          // включение/выключение ВРЧ

signals:
  void usdSettingsChenged();
  void errorValueInputed(QString stringValue);

private slots:
  void onUserChangeSettings();
  void onChangeAStartEdit(QString text);
  void onVrchTableChanged(int row,int col);

private: 

  double rangeKof;
  double delayKof;

private:

  AUsdControlParams m_usdControlParams;

//созданий виджетов
  void formCreate();
  ANamedLineEdit * addIntValidateNameLineEditToLayout(QString name,QLayout *layout,int bottom,int top);
  ANamedLineEdit * addDoubleValidateNameLineEditToLayout(QString name,QLayout *layout,double bottom,double top);

  ANamedLineEdit *m_ganeEdit;
  ANamedLineEdit *m_rangeEdit;
  ANamedLineEdit *m_delayEdit;
  ANamedLineEdit *m_rejectEdit;
  ANamedLineEdit *m_aThreshEdit;
  ANamedLineEdit *m_aStartEdit;
  ANamedLineEdit *m_aWidthEdit;
  ANamedLineEdit *m_frecuencyEdit;
  ANamedLineEdit *m_pulseCountEdit;
//  ANamedLineEdit *gziHiEdit; Не используется, так как он всегда 1 (200 В) для этого прибора
  ANamedLineEdit *m_timerFreqEdit;

  QLabel *m_typePointLabel;
  QComboBox *m_typePointBox;
  QHBoxLayout *m_typePointLayout;

  QLabel *m_polosaLabel;
  QComboBox *m_polosaBox;
  QHBoxLayout *m_polosaLayout;

  QLabel *m_detectorLabel;
  QComboBox *m_detectorBox;
  QHBoxLayout *m_detectorLayout;

  QLabel *m_includeVrchLabel;
  QComboBox *m_includeVrchBox;
  QHBoxLayout *m_includeVrchLayout;

  ANamedLineEdit *m_vrchPointsEdit;
  QVBoxLayout *m_settingsLayout;

  QTableWidget *m_vrchTable;

  QHBoxLayout *m_mainLayout;

  void closeEvent(QCloseEvent *event);
//----------------

//блок методов по работе с настройками прибора
  void verifyUsdSettings();
  void reedUsdSettings();
  void writeUsdSettings();
  QString setingsFileName();
  bool loadUsd60Settings();
  void saveSettings();
  void setStandartSettings();
};

#endif // AUSD60_SETTINGS_H
