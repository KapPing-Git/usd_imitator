#ifndef AINSERTER_H
#define AINSERTER_H

#include <QWidget>

class AInserter : public QWidget
{
  Q_OBJECT
public:
  explicit AInserter(int numInterval,QWidget *parent = 0);
  int num();

signals:
  void mousePressed(AInserter *inserter);

public slots:

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);

private:
  int m_num;
};

#endif // AINSERTER_H
