#ifndef ANAMEDLINEEDIT_H
#define ANAMEDLINEEDIT_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

class ANamedLineEdit : public QWidget
{
  Q_OBJECT
public:
  explicit ANamedLineEdit(QWidget *parent = 0,QString name="");
  QLineEdit *lineEdit;
  
signals:
  
public slots:

private:
  QLabel *nameLabel;
  QHBoxLayout mainLayout;
  
};

#endif // ANAMEDLINEEDIT_H
