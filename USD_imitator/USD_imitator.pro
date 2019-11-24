#-------------------------------------------------
#
# Project created by QtCreator 2014-05-22T09:57:28
#
#-------------------------------------------------

QT       += core gui script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = USD_imitator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ausd.cpp \
    aintervalshower.cpp \
    ainterval.cpp \
    ainserter.cpp \
    ausd60n.cpp \
    ../AChart/achartpolyline.cpp \
    ../AChart/achartseries.cpp \
    ../AChart/achartseriescollection.cpp \
    ../AChart/achart.cpp \
    ../AChart/achartaxis.cpp \
    ../AChart/achartaxiscollection.cpp \
    ../AChart/achartmousephysicalcoord.cpp \
    ../AChart/achartpoint.cpp \
    ../AChart/AChartAxisPainter/achartaxispainter.cpp \
    ../AChart/AChartAxisPainter/achartaxispaintersimple.cpp \
    ../AChart/ACHartDevice/achartdecorator.cpp \
    ../AChart/ACHartDevice/achartdecoratorcontainer.cpp \
    ../AChart/ACHartDevice/achartseriesdecorator.cpp \
    ausd60Settings.cpp \
    anamedlineedit.cpp \
    aintervalcontainer.cpp \
    aformulaselector.cpp \
    aformulacreator.cpp \
    valueGivers/aabstractvaluegiver.cpp \
    valueGivers/alistformulasvaluegiver.cpp

HEADERS  += mainwindow.h \
    ausd.h \
    aintervalshower.h \
    ainterval.h \
    ainserter.h \
    ausd60n.h \
    ../AChart/achartpolyline.h \
    ../AChart/achartseries.h \
    ../AChart/achartseriescollection.h \
    ../AChart/achart.h \
    ../AChart/achartaxis.h \
    ../AChart/achartaxiscollection.h \
    ../AChart/achartmousephysicalcoord.h \
    ../AChart/achartpoint.h \
    ../AChart/AChartAxisPainter/achartaxispainter.h \
    ../AChart/AChartAxisPainter/achartaxispaintersimple.h \
    ../AChart/ACHartDevice/achartdecorator.h \
    ../AChart/ACHartDevice/achartdecoratorcontainer.h \
    ../AChart/ACHartDevice/achartseriesdecorator.h \
    ausd60Settings.h \
    anamedlineedit.h \
    aintervalcontainer.h \
    aformulaselector.h \
    aformulacreator.h \
    valueGivers/aabstractvaluegiver.h \
    valueGivers/alistformulasvaluegiver.h

FORMS    += mainwindow.ui \
    ausd.ui \
    aintervalshower.ui \
    aformulaselector.ui \
    aformulacreator.ui

TRANSLATIONS += ru.ts

OTHER_FILES += \
    todo.txt
