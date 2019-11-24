#include <QApplication>
#include <QTranslator>
#include <QPointer>
#include <QMessageBox>
#include "mainwindow.h"
#include "ausd.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  // вызываем поддержку перевода
  QPointer<QTranslator> qt_translator = new QTranslator;
  QString file_translate = QApplication::applicationDirPath () + "/ru.qm";
  if ( !qt_translator->load(file_translate))
    {
//      QMessageBox::warning(NULL, QWidget::tr("Внимание"),
//                                     QWidget::tr("отсутствует файл языковой поддержки ru.qm"),
//                                     QMessageBox::Cancel,QMessageBox::Cancel);
      delete qt_translator.data();
    }
  else
    {
      qApp->installTranslator( qt_translator.data() );
    }

  MainWindow w;
  w.show();

  int errorCode = a.exec();
  if(qt_translator) delete(qt_translator.data());
  return errorCode;
}
