#ifndef AUSD60N_H
#define AUSD60N_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include "ausd60Settings.h"
#include "ausd.h"

class AUsd60N : public AUsd
{
  Q_OBJECT
public:
  explicit AUsd60N(QWidget *parent = 0);
  ~AUsd60N();
  void start();
  void stop();
  int trendSize();
  qreal maxshum();
  qreal maxVremyaProhogdeniya();
  qreal maxzatoohanie();

signals:

public slots:
  void viewSettings();
  void onUsdSettingsChanged();

private:
#define USD60_TCP_PORT (quint16) 0xCD60  // TCP Порт УСД60
#define USD60_UDP_PORT (quint16) 7500  // UDP Порт на который УСД60 будет посылать пакеты
#define USD60_SET_SETTINGS_COMMAND (quint32)0x0100F001
#define USD60_VERIFY_SET_SETTINGS_COMMAND (quint32)0x0100100F
#define USD60_SET_CONTROLLER_MODE (quint32)0xCD60FCFC
#define USD60_VERIFY_SET_CONTROLLER_MODE (quint32)0xFCFC
#define POINT_COUNT 480

  bool m_controllerMode;
  AUSD60_Settings *m_settings;
  QTcpSocket *m_tcpSocket;
  QTcpServer *m_server;
  QUdpSocket *m_udpSocket;
  QTimer m_timer;
  AChartSeries *m_oregenalSeries;
  int m_oldTimerFreq;

  void setControllerMode(bool aControllerMode);
  bool controllerMode();

private slots:
  void onTCPReadyRead();
  void onNewConnection();
  void onDisconnected();
  void onInputedErrorSettings(QString errString);
  void sendA_Scan();

};

#endif // AUSD60N_H
