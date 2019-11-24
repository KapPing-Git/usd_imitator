#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  setWindowTitle(tr("Ultrasonic devise imitator (Kropus)"));
  ui->tabWidget->clear();
  ui->tabWidget->addTab(new AUsd60N(this),tr("УСД60NImitator"));
}

MainWindow::~MainWindow()
{
  delete ui;
}

QString MainWindow::settingsDir()
{
  QString myName = tr("УСД60NImitator");
#ifdef Q_OS_LINUX
  myName = "." + myName;
#endif
  return QDir::homePath() + QDir::separator() + myName;
}
