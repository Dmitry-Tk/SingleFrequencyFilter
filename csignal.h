#ifndef CSIGNAL_H
#define CSIGNAL_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <complex>
#include "../../Arxiv/cfourierfilter.h"
#include "cfourierfilt.h"

#define Nlines 3

class CSignal : public QObject
{
    Q_OBJECT

public:
    CSignal(size_t N = 200, double SamplingFreq = 1e6/*[Гц]*/, size_t K = 3, int TimerInterval = 1);
    ~CSignal();

    void createSignalSpectr();
    void getSignalSpectr(QVector<QPointF>& spectr);
    double getSpectrComp(double freq);
    void resetData();
    double getMaxAmp()
    {
        return MaxAmp;
    }
    double getSamplingFreq()
    {
        return SamplingFreq;
    }
    QVector<QPointF> data[3];
    QVector<QPointF> SC[2];/**< детектированная спектральная информация */
    std::vector<double> freq, Amp;/**< параметры спектра модельного сигнала */
    int idTimer;
    bool simulation;
    CFourierFilter* FourierFilter;
    CFourierFilt* FourierFilt;
    size_t K;/**< число компонент в спектре */

public slots:
    /// старт/стоп генерации сигнала с фильтрацией
    void onBtnPressed(bool);
    /// свойства генерируемого сигнала
    void setK(int);
    void setPosFirstSC(double);
    void setSignalAmp(double);
    void setRandomSignalSpectr(int);
    void setNoiseValue(double);

signals:
    void DataUpdated();
    void SCDUpdated(double);
    void SpectrGenerated();

private:
    void timerEvent(QTimerEvent*);

    double relNoiseValue;
    bool boolRandomSpectr;
    std::vector<double> w;
    int TimerInterval;
    double shift;/**< сдвиг относительно индекса анализируемой частотной компоненты Фурье-преобразования */
    int position;/**< индекс анализируемой частотной компоненты Фурье-преобразования */
    size_t N;/**< число отсчётов в буфере сигнала */
    double posFirstSC;
    double MaxAmp;
    double t;
    double SamplingPeriod;/**< период дискретизации */
    double SamplingFreq;/**< частота дискретизации */
    double sumFiltSpectralComp[2];
    double maxFilterSpectrValue;
    uint index;
};

#endif // CSIGNAL_H
