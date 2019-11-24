#include "anamedlineedit.h"

ANamedLineEdit::ANamedLineEdit(QWidget *parent, QString name) :
  QWidget(parent)
{
  nameLabel=new QLabel(name,this);
  lineEdit=new QLineEdit(this);
  mainLayout.addWidget(nameLabel);
  mainLayout.addWidget(lineEdit);
  setLayout(&mainLayout);
}
