#ifndef QtPlotter_H
#define QtPlotter_H

#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QLogValueAxis>
#include "csignal.h"

enum TData{Sig, Spectr};

using namespace QtCharts;

class QtPlotter : public QChartView
{
    Q_OBJECT
    TData type;
    QAbstractAxis*  axisX;
    QAbstractAxis*  axisY;
    QValueAxis* ValueAxisY;
    QLogValueAxis* LogValueAxisY;
    bool boolAutoScale;
    bool boolLogAxisY;
    bool what = true;

public:
    explicit QtPlotter(QWidget *parent = nullptr, TData type = Sig);
    void CreateAxesAndSeries();

    QChart chart;
    QLineSeries* line[Nlines];
    QLineSeries FilterSpectr[Nlines-1];
    QScatterSeries detectedSC[2];
    QScatterSeries RealSpectr;
    CSignal* Signal;
    double MaxFilterSpectrValue;

public slots:
    void updateSignalChart();
    void onFiltFreqUpdated(double);
    void onSpectrUpdated();
    void setNewYAxis(int);
    void setAutoScale(int);
};

#endif // QtPlotter_H
