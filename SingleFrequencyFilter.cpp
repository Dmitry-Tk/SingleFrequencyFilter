#include <qobject.h>
#include "qtplotter.h"
#include <QtWidgets>
#include <QChartView>
#include <QChart>
#include "csignal.h"
#include "cwidget.h"

/**
 * Одночастотная фильтрация                                         *
 * Цель - выяснить насколько точно можно оценивать спектральную     *
 * компоненту в сложном регулярном (с неизменным спектром) сигнале  *
 * ПРОВЕРИТЬ со статическим массивом в классе ???
**/

#define W 00
#define H 300

using namespace QtCharts;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow MainWindow;
    QDockWidget dock;
    CWidget CtrlWgt;
    QDockWidget DockWgtPlotter;
    QtPlotter SignalPlotter(nullptr, Sig), SpectrPlotter(nullptr, Spectr);

    dock.setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    dock.setFeatures( QDockWidget::DockWidgetMovable );

    dock.setWidget( &CtrlWgt );
    MainWindow.addDockWidget(Qt::TopDockWidgetArea, &dock);
    /// инсталлируем виджет плоттера сигнала
    SignalPlotter.resize(W, H);
    MainWindow.setCentralWidget( &SignalPlotter );
    /// инсталлируем виджет плоттера спектра
    DockWgtPlotter.setWidget( &SpectrPlotter );
    DockWgtPlotter.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    DockWgtPlotter.setAllowedAreas(Qt::BottomDockWidgetArea);// | Qt::TopDockWidgetArea);
    DockWgtPlotter.setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
//    DockWgtPlotter.setWindowFlags( Qt::FramelessWindowHint );

    SpectrPlotter.resize(W, H);
    MainWindow.addDockWidget(Qt::BottomDockWidgetArea, &DockWgtPlotter);
    ///
    MainWindow.setWindowTitle("Single-frequency filter");
    MainWindow.resize(700, 1300);
    MainWindow.show();
    /// передаем указатель к массиву данных сигнала
    /// и фильтруемой спектральной компоненты
    SignalPlotter.Signal = CtrlWgt.Signal;
    SpectrPlotter.Signal = CtrlWgt.Signal;
    ///
    /// Signal -> SignalPlotter / SpectrPlotter
    QObject::connect( CtrlWgt.Signal, SIGNAL(DataUpdated()), &SignalPlotter, SLOT(updateSignalChart()) );
    QObject::connect( CtrlWgt.Signal, SIGNAL(DataUpdated()), &SpectrPlotter, SLOT(updateSignalChart()) );
    QObject::connect( &CtrlWgt, SIGNAL(FiltFreqUpdated(double)), &SpectrPlotter, SLOT(onFiltFreqUpdated(double)) );
    QObject::connect( CtrlWgt.Signal, SIGNAL(SpectrGenerated()), &SpectrPlotter, SLOT(onSpectrUpdated()) );

    QObject::connect( CtrlWgt.Signal, SIGNAL(SCDUpdated(double)), &CtrlWgt, SLOT(updateSpectrCompDetectValue(double)) );
    /// управление отображением спектров
    QObject::connect( CtrlWgt.chboxLogY, SIGNAL(stateChanged(int)), &SpectrPlotter, SLOT(setNewYAxis(int)) );
    QObject::connect( CtrlWgt.chboxAutoScale, SIGNAL(stateChanged(int)), &SignalPlotter, SLOT(setAutoScale(int)) );
    QObject::connect( CtrlWgt.chboxAutoScale, SIGNAL(stateChanged(int)), &SpectrPlotter, SLOT(setAutoScale(int)) );
    ///
    SignalPlotter.CreateAxesAndSeries();
    SpectrPlotter.CreateAxesAndSeries();

    CtrlWgt.spboxNSp->setValue( 128 );
    CtrlWgt.sliderFC->setValue( 64 );
    CtrlWgt.spboxNoise->setValue( 0.1 );
    CtrlWgt.spboxK->setValue( 3 );
    CtrlWgt.spboxSignalAmp->setValue( 0.25 );
    CtrlWgt.spboxPosFirstSC->setValue( 0.0 );
    CtrlWgt.chboxRandomSpectr->click();


    return a.exec();
}
