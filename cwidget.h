#ifndef CWIDGET_H
#define CWIDGET_H

#include <QObject>
#include <QWidget>
#include <QtWidgets>
#include "../../Arxiv/cfourierfilter.h"
#include "cfourierfilt.h"
#include "csignal.h"


class CWidget : public QWidget
{
    Q_OBJECT

public:
    CWidget();
    ~CWidget();
    /// общее управление
    QPushButton* btnpause;
    QCheckBox* chboxLogY;
    QCheckBox* chboxAutoScale;
    QDoubleSpinBox* spboxNoise;
    /// управление фильтром
    QSpinBox* spboxNSp;
    QSlider* sliderFC;
    QDoubleSpinBox* spboxShiftFC;
    /// управление спектром сигнала
    QSpinBox* spboxK;
    QDoubleSpinBox* spboxPosFirstSC;
    QDoubleSpinBox* spboxSignalAmp;
    QCheckBox* chboxRandomSpectr;
    /// вывод детектированных и спектральных данных
    QLabel* lblSpectrComp;
    QLabel* lblSpectrCompDetected;
    QTableWidget SpectraData;
    /// сигнал
    CSignal* Signal;

    const double SamplingPeriod = 1e-6;
    const double SamplingFreq = 1.0 / SamplingPeriod;
    const double gT = 0.0001;

public slots:
    void setSliderMaximum(int);
    void setToolTip(int);

    void setNSp(int NSp);
    void setFiltFreq();
    void updateSpectrCompDetectValue(double);
    void onSpectrUpdated();
signals:
//    void SpectrGenerated();
    void FiltFreqUpdated(double);
    void FiltFreqChanged();
};

#endif // CWIDGET_H
