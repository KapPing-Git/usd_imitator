#include "ainserter.h"
#include <QPainter>
#include <QRect>

AInserter::AInserter(int numInterval, QWidget *parent) :
  QWidget(parent)
{
  m_num = numInterval;
  setMaximumWidth(30);
  setMinimumWidth(30);
  setMinimumHeight(50);
  setToolTip("Вставить");
}

int AInserter::num()
{
  return m_num;
}

void AInserter::paintEvent(QPaintEvent *event)
{
  event = event;
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  int indent = 5;
  int middleX = width() /2;
  int middleY = height() / 2;
  painter.drawLine(middleX, middleY-middleX, middleX, middleY + middleX);
  painter.drawLine(indent,middleY,width() - indent,middleY);
  QRect leftRect(0,0,indent,height());
  QRect rightRect(width() - indent,0,indent,height());
  painter.drawArc(leftRect,16*270,16*180);
  painter.drawArc(rightRect,16*90,16*180);
}

void AInserter::mousePressEvent(QMouseEvent *event)
{
  if (event)
  emit mousePressed(this);
}

