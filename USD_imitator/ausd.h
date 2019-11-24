#ifndef AUSD_H
#define AUSD_H

#include <QWidget>
#include <QTimer>
#include <QDoubleValidator>
#include "ui_ausd.h"
#include "../AChart/achart.h"
#include "../AChart/achartseries.h"
#include "aformulaselector.h"
#include "valueGivers/alistformulasvaluegiver.h"

namespace Ui {
  class AUsd;
}

class AUsd : public QWidget
{
  Q_OBJECT

public:
  explicit AUsd(QWidget *parent = 0);
  ~AUsd();
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual int trendSize() = 0;
  virtual qreal maxzatoohanie() = 0;
  virtual qreal maxVremyaProhogdeniya() = 0;
  virtual qreal maxshum() = 0;
  void addToLog(QString logString);
  void clearLog();
  qreal frecuency() const;
  void setFrecuency(const qreal &frecuency);

  qreal datchikFrecuency() const;
  void setDatchikFrecuency(const qreal &datchikFrecuency);

public slots:
  virtual void viewSettings() = 0;

protected:
  AFormulaSelector *m_formulaSelector;
  Ui::AUsd *ui;

  AChartSeries *series();
  AChart *chart();
  qreal zatoohanie();
  void setZatoohanie(qreal value);
  qreal zadergka();
  void setZadergka(qreal value);
  qreal shum();
  void setShum(qreal value);

  QString mainSettingsPath();

private slots:

  void on_settingsButton_clicked();

  void on_startButton_clicked();

  void on_stopButton_clicked();

  void on_powerSlider_sliderMoved(int position);

  void on_startTimeSlider_sliderMoved(int position);

  void on_shumSlider_sliderMoved(int position);

  void on_datchikFrecuencyEdit_editingFinished();

  void on_powerFormulaButton_clicked();

  void on_startTimeFormulaButton_clicked();

  void on_shumFormulaButton_clicked();

  void onPowerNewValueReady(qreal value);
  void onTimeNewValueReady(qreal value);
  void onShumNewValueReady(qreal value);
  void onAnyValueGiverStoped();

  void on_autoReplayButton_clicked(bool checked);

  void on_autoRepeateBox_clicked(bool checked);

private:
  qreal m_frecuency;
  qreal m_datchikFrecuency;
  AChartSeries *m_series;

  qreal m_zatuhanie;
  qreal m_zadergka;
  qreal m_shum;

  AIntervalContainer m_powerFormula;
  AIntervalContainer m_timeFormula;
  AIntervalContainer m_shumFormula;
  AAbstractValueGiver *m_powerValueGiver;
  AAbstractValueGiver *m_timeValueGiver;
  AAbstractValueGiver *m_shumValueGiver;
};

#endif // AUSD_H
