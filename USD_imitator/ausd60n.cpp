#include "ausd60n.h"

AUsd60N::AUsd60N(QWidget *parent) :
  AUsd(parent)
{
  m_controllerMode = false;
  m_oldTimerFreq = 0;
  m_settings = new AUSD60_Settings();
  connect(m_settings,&AUSD60_Settings::usdSettingsChenged,this,&AUsd60N::onUsdSettingsChanged);
  connect(m_settings,&AUSD60_Settings::errorValueInputed,this,&AUsd60N::onInputedErrorSettings);
  setFrecuency(m_settings->frecuency());
  onUsdSettingsChanged();
  connect(&m_timer,&QTimer::timeout,this,&AUsd60N::sendA_Scan);

  //создаёт и настраиваем объекты для соединения с прибором
  m_server = new QTcpServer(this);
  m_server->listen(QHostAddress::Any,USD60_TCP_PORT);
  connect(m_server,&QTcpServer::newConnection,this,&AUsd60N::onNewConnection);
  m_udpSocket = new QUdpSocket(this);

  ui->powerSlider->setRange(0,1000);

  m_oregenalSeries = new AChartSeries(this,tr("start"));
  m_oregenalSeries->setPointCount(POINT_COUNT);
  series()->setPointCount(POINT_COUNT);
}

AUsd60N::~AUsd60N()
{
}

void AUsd60N::start()
{
  m_timer.start();
}

void AUsd60N::stop()
{
  m_timer.stop();
}

int AUsd60N::trendSize()
{
  return POINT_COUNT;
}

qreal AUsd60N::maxshum()
{
  return 12.5;
}

qreal AUsd60N::maxVremyaProhogdeniya()
{
  return 6000;
}

qreal AUsd60N::maxzatoohanie()
{
  return 180;
}

void AUsd60N::viewSettings()
{
  m_settings->show();
}

void AUsd60N::onUsdSettingsChanged()
{
  if (m_oldTimerFreq != m_settings->timer_freq())
    {
      m_timer.setInterval(1000 * ((qreal)1/m_settings->timer_freq()));
      m_oldTimerFreq = m_settings->timer_freq();
    }
  setFrecuency(m_settings->frecuency());
  if (m_settings->detector() == AUSD60_Settings::detectorEnum_radio)
    chart()->setPhysicalView(0,-125,m_settings->range(),125);
  else
    chart()->setPhysicalView(0,0,m_settings->range(),125);
}

void AUsd60N::setControllerMode(bool aControllerMode)
{
  m_controllerMode = aControllerMode;
}

bool AUsd60N::controllerMode()
{
  return m_controllerMode;
}

void AUsd60N::onTCPReadyRead()
{
  if (controllerMode())
    {
      quint32 command = 0;
      AUSD60_Settings::AUsdControlParams params;
      m_tcpSocket->read((char*)&command,sizeof(command));
      if (command == USD60_SET_SETTINGS_COMMAND)
        {
          m_tcpSocket->read((char*)&params,sizeof(params));
          m_settings->setParams(params);
          quint32 podtvergdenie = USD60_VERIFY_SET_SETTINGS_COMMAND;
          m_tcpSocket->write((char*)&podtvergdenie,sizeof(podtvergdenie));
          onUsdSettingsChanged();
        }
    }
  else
    {
      quint32 command = 0;
      m_tcpSocket->read((char*)&command,sizeof(quint32));
      if (command == USD60_SET_CONTROLLER_MODE)
        {
          setControllerMode(true);
          quint32 podtvergdenie = USD60_VERIFY_SET_CONTROLLER_MODE;
          m_tcpSocket->write((char*)&podtvergdenie,sizeof(podtvergdenie));
        }
    }
}

void AUsd60N::onNewConnection()
{
  m_tcpSocket = m_server->nextPendingConnection();
  connect(m_tcpSocket,&QTcpSocket::disconnected,this,&AUsd60N::onDisconnected);
  connect(m_tcpSocket,&QTcpSocket::readyRead,this,&AUsd60N::onTCPReadyRead);
}

void AUsd60N::onDisconnected()
{
  m_tcpSocket->deleteLater();
  setControllerMode(false);
}

void AUsd60N::onInputedErrorSettings(QString errString)
{
  addToLog(errString);
}

void AUsd60N::sendA_Scan()
{
#define DEFAULT_MAIN_AMPLITUDA 12
#define TIME_KOEF 10;

  //формируем график
  qreal dT = m_settings->range() / POINT_COUNT;
  qreal beginX = m_settings->delay();
  qreal koefShum = shum() / RAND_MAX;
  qreal zatohanie = zatoohanie();
  qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());
  //рассчитываем длинну и амплитуду полезного сигнала
  qreal lengthNeedsSignal = 1000 * ((1 / frecuency()) * m_settings->pulse_count()) * 2;
  qreal mainAmlituda = (DEFAULT_MAIN_AMPLITUDA * m_settings->pulse_count()) / (1 + qAbs(m_settings->frecuency() - datchikFrecuency()));
  qreal x = beginX;
  for (int numPoint = 0;numPoint < POINT_COUNT;numPoint++)
    {
      //добавляем составляющую шума
      qreal y = qrand() * koefShum - (shum() / 2);

      //добавляем составляющую полезного сигнала
      if ( (x > zadergka()) && (x < zadergka() + lengthNeedsSignal) )
        {
          //для получения нужной точки графика берём точку главной синусоиды
          //и используем её как амплитуду для построения сигнала
          qreal sigAmpliduda = mainAmlituda * qSin((1/lengthNeedsSignal) * M_PI * (x - zadergka()));
          qreal sigY = sigAmpliduda * qSin((frecuency()/1000) * 2 * M_PI * (x - zadergka()));
          sigY /= zatohanie;
          y += sigY;
        }

      //масштабируем сигнал
      y *= (1 + m_settings->gain());

      m_oregenalSeries->setPoint(numPoint,x - m_settings->delay(),y);
      x += dT;
    }

  // получаем модифицируемый в соответствии с типом детектора и обрезанный график
  series()->startTransaction();
  if (m_settings->detector() == AUSD60_Settings::detectorEnum_full)
    {
      for (int i = 0;i < POINT_COUNT;i++)
        {
          qreal x = m_oregenalSeries->point(i).x();
          qreal y = m_oregenalSeries->point(i).y();
        }
    }
  if (m_settings->detector() == AUSD60_Settings::detectorEnum_minus)
    {
      for (int i = 0;i < POINT_COUNT;i++)
        {
          qreal x = m_oregenalSeries->point(i).x();
          qreal y = m_oregenalSeries->point(i).y();
          if (y > 0) y = 0;
          if (y < 0) y *= -1;
          if (y > 128) y = 125;
          if (qAbs(y) < m_settings->reject()) y = 0;

          series()->setPoint(i,x,y);
        }
    }
  if (m_settings->detector() == AUSD60_Settings::detectorEnum_plus)
    {
      for (int i = 0;i < POINT_COUNT;i++)
        {
          qreal x = m_oregenalSeries->point(i).x();
          qreal y = m_oregenalSeries->point(i).y();
          if (y < 0) y = -0;
          if (y > 128) y = 125;
          if (qAbs(y) < m_settings->reject()) y = 0;

          series()->setPoint(i,x,y);
        }
    }
  if (m_settings->detector() == AUSD60_Settings::detectorEnum_radio)
    {
      //здесь только обрезать
      for (int i = 0;i < POINT_COUNT;i++)
        {
          qreal x = m_oregenalSeries->point(i).x();
          qreal y = m_oregenalSeries->point(i).y();
          if (y < -125) y = -125;
          if (y > 128) y = 125;
          if (qAbs(y) < m_settings->reject()) y = 0;

          series()->setPoint(i,x,y);
        }

    }
  series()->stopTransaction();
  chart()->update();

  //находим точку начала роста
  int m_time = 0;
  for (int i = 0;i < series()->pointsCount();i++)
    {
      qreal x = series()->point(i).x();
      qreal y = series()->point(i).y();
      if (y > 0)
        {
          m_time = x * TIME_KOEF;
          break;
        }
    }

  //находим максимальное значение в заданном диапазоне
  qint16 m_maxA = 0;
  int begin = series()->indexOf(m_settings->a_start());
  int end = series()->indexOf(m_settings->a_start() + m_settings->a_width());
  for (int i = begin;i < end;i++)
    {
      qreal y = qAbs(series()->point(i).y());
      if (m_maxA < y) m_maxA = y;
    }

  //формируем массив и посылаем его по UDP
  if (controllerMode())
    {
#define BUF_SIZE 1024
#define START_MASSIV 16
      qint16 data[BUF_SIZE];
      memset(data,0,sizeof(data));

      data[0] = m_maxA;
      data[1] = m_time;
      data[2] = m_time >> 16;
      for (int i = 0;i < POINT_COUNT;i++)
        {
          qreal y = series()->point(i).y();
          qint16 minY = 0;
          qint16 maxY = 0;
          if (m_settings->detector() == AUSD60_Settings::detectorEnum_full)
            {
              minY = y;
              maxY = 0;
            }
          else
            {
              minY = y - shum() / 2;
              maxY = y + shum() / 2;
            }
          data[START_MASSIV + i * 2] = minY;
          data[START_MASSIV + i * 2 + 1] = maxY;
        }
      QHostAddress localHost = QHostAddress::LocalHost;
      m_udpSocket->writeDatagram((char *)data,sizeof(data),localHost,USD60_UDP_PORT);
    }
}
