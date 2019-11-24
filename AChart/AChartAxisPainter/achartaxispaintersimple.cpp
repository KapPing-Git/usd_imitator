#include "achartaxispaintersimple.h"
#include <QPainter>
#include <math.h>
#include <QLinearGradient>

AChartAxisPainterSimple::AChartAxisPainterSimple(AChart *parent) :
  AChartAxisPainter(parent)
{
  m_gridSize.setHeight(100);
  m_gridSize.setWidth(100);
}

QRect AChartAxisPainterSimple::draw(QPaintDevice *paintDevice, QRect paintRect)
{
#define INDENT 5
  QPainter painter(paintDevice);
  // рассчитываем высоту поля для  вывода оси X
  // считаем кол-во видимых осей
  int colVisibleAxis = 0;
  QList<int> visibleAxisNum;
  for (int i = 0;i < m_chart->axisContainer()->count();i++)
    {
      if (m_chart->axisContainer()->axis(i)->visible())
        {
          visibleAxisNum << colVisibleAxis;
          colVisibleAxis++;
        }
      else
        visibleAxisNum << -1;
    }
  int downIndent = (m_chart->fontMetrics().height() + INDENT) * colVisibleAxis;
//  int downIndent = (m_chart->fontMetrics().height() + INDENT) * m_chart->axisContainer()->count();
  paintRect.adjust(0,0,0,-downIndent);
  painter.setClipRect(paintRect,Qt::ReplaceClip);

  // Рисуем оси по У
  //// проверяем не будут ли значения оси налезать друг на друга
  if (m_gridSize.height() < m_chart->fontMetrics().height()) m_gridSize.setHeight(m_chart->fontMetrics().height());

  m_axisRect.clear();
  AChartAxisContainer *axisContainer = m_chart->axisContainer();
  for (int i = 0;i < axisContainer->count();i++)
    {
      AChartAxis *axis = axisContainer->axis(i);
      if (!axis->visible()) continue;
      qreal screenPerPhysY = (qreal)paintRect.height() / axis->viewHeight();
      // определяем начало рисования координатной оси
      qreal delta = (qreal)m_gridSize.height() / screenPerPhysY;
      //делаем так, чтобы оси чётко совпадали с округлёнными значениями
      QString strDelta = QString::number(delta,m_yFormat,m_yPrecision);
      delta = strDelta.toDouble();
      if (delta == 0) delta = 1/pow(10,m_yPrecision);
      qreal startY = - fmod(axis->viewMinY() , delta);

      //прорисовываем ось и определяем её ширину в пикселях
      QPen pen(axis->color());
      pen.setStyle(Qt::DotLine);
      int axisWidth = 0;
      for (qreal y = axis->viewMinY() + startY;y < axis->viewMaxY();y += delta)
        {
          int screenY = paintRect.bottom() - round((y - axis->viewMinY()) * screenPerPhysY);
          QString s = QString::number(y,m_yFormat,m_yPrecision);
          painter.setPen(pen);
          painter.drawText(paintRect.x(),screenY,s);
          painter.drawLine(paintRect.x(),screenY,paintRect.right(),screenY);
          if (m_chart->fontMetrics().width(s) > axisWidth) axisWidth = m_chart->fontMetrics().width(s);
        }
      QRect currAxisRect = QRect(paintRect.left(),paintRect.top(),axisWidth,paintRect.height());

      //обозначаем ось полупрозрачным градиентом
      QLinearGradient gradient(currAxisRect.left(),currAxisRect.center().y(),currAxisRect.right(),currAxisRect.center().y());
      int r = 0;
      int g = 255;
      int b = 0;
      float z = 0.5;//разность между начальным и конечным цветом градиента
      int alfa = 50;
      if (axis != m_chart->axisContainer()->activeAxis())
        {
          gradient.setColorAt(1, QColor(r * z,g * z,b * z,alfa));
          gradient.setColorAt(0.5, QColor(r,g,b,alfa));
          gradient.setColorAt(0, QColor(r * z,g * z,b * z,alfa));
        }
      else
        {
          gradient.setColorAt(1, QColor(r,g,b,alfa));
          gradient.setColorAt(0.5, QColor(r * z,g * z,b * z,alfa));
          gradient.setColorAt(0, QColor(r,g,b,alfa));
        }

      QBrush brush = QBrush(gradient);
      painter.fillRect(currAxisRect,brush);

      m_axisRect << currAxisRect;
      paintRect.adjust(axisWidth + INDENT,0,0,0);

    }
  // Рисем оси по X
  painter.setClipRect(paintRect.adjusted(0,0,0,downIndent),Qt::ReplaceClip);
  for (int i = 0;i < axisContainer->count();i++)
    {
      AChartAxis *axis = axisContainer->axis(i);
      if (!axis->visible()) continue;
      qreal screenPerPhysX = (qreal)paintRect.width() / axis->viewWidth();
      // определяем начало рисования координатной оси
      qreal delta = (qreal)m_gridSize.width() / screenPerPhysX;
      qreal startX = - fmod(axis->viewMinX() , delta);
      //проверяем не будут ли значения налезать друг на друга, если будут, то увеличиваем сетку
      int maxWidth = m_gridSize.width();
      for (qreal x = axis->viewMinX() + startX;x < axis->viewMaxX();x += delta)
        {
          QString s = QString::number(x,m_xFormat,m_xPrecision);
          if ((m_chart->fontMetrics().width(s) + INDENT) > maxWidth) maxWidth = m_chart->fontMetrics().width(s) + INDENT;
        }
      delta = (qreal)maxWidth / screenPerPhysX;
      //делаем так, чтобы оси чётко совпадали с округлёнными значениями
      QString strDelta = QString::number(delta,m_xFormat,m_xPrecision);
      delta = strDelta.toDouble();
      if (delta == 0) delta = 1/pow(10,m_xPrecision);
      startX = - fmod(axis->viewMinX() , delta);
      //прорисовываем ось и определяем её ширину в пикселях
      QPen pen(axis->color());
      pen.setStyle(Qt::DotLine);
      for (qreal x = axis->viewMinX() + startX;x < axis->viewMaxX();x += delta)
        {
          int screenX = paintRect.left() + round((x - axis->viewMinX()) * screenPerPhysX);
          QString s = QString::number(x,m_xFormat,m_xPrecision);
          painter.setPen(pen);
          int numVisAxis = visibleAxisNum[i];
          painter.drawText(screenX,paintRect.bottom() + (colVisibleAxis*(m_chart->fontMetrics().height() + INDENT)) - numVisAxis*(m_chart->fontMetrics().height() + INDENT) ,s);
//          painter.drawText(screenX,paintRect.bottom() + (axisContainer->count()*(m_chart->fontMetrics().height() + INDENT)) - numVisAxis*(m_chart->fontMetrics().height() + INDENT) ,s);
          painter.drawLine(screenX,paintRect.top(),screenX,paintRect.bottom());
        }
    }
  return paintRect;
}

void AChartAxisPainterSimple::mouseMoveEvent(QMouseEvent *event)
{
  event = event;
}

void AChartAxisPainterSimple::mousePressEvent(QMouseEvent *event)
{
  //при нажатии на левую клавишу делаем соответствующую ось активной
  if (event->button() == Qt::LeftButton)
    {
      for (int i = 0;i < m_chart->axisContainer()->count();i++)
        {
          if (m_axisRect[i].contains(event->pos()))
            {
              m_chart->axisContainer()->setActiveAxis(i);
              break;
            }
        }
      m_chart->update();
    }
}

void AChartAxisPainterSimple::mouseReleaseEvent(QMouseEvent *event)
{
  event = event;
}

void AChartAxisPainterSimple::wheelEvent(QWheelEvent *event)
{
  event = event;
}

