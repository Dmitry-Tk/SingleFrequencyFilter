#include "csignal.h"
#include <cmath>
#include <qdebug.h>
#include <iostream>
#include <iomanip>
#include <QtCore/QRandomGenerator>

/// приход сигнала блоками / или по одному отсчёту
#define BlockProcessing

using namespace std;
///
/// диапазон относительных частот спектра сигнала f: [0 ... 0.5*SamplingFreq)
CSignal::CSignal(size_t N, double SamplingFreq, size_t K, int TimerInterval)
{
    this->N = N;
    this->SamplingFreq = SamplingFreq;
    this->K = K;
    this->TimerInterval = TimerInterval;
    ///
    SamplingPeriod = 1.0 / SamplingFreq;
    Amp.resize( K );
    freq.resize( K );
    w.resize( K );
    for (int k = 0; k < Nlines; k++)
        data[k].resize( N );
    SC[0].resize( 2 );
    SC[1].resize( 2 );
}

CSignal::~CSignal()
{
    Amp.clear();
    freq.clear();
    w.clear();
    for (int k = 0; k < Nlines; k++)
        data[k].resize( N );
    for (int k = 0; k < 2; k++)
        SC[k].clear();
    if (FourierFilter != nullptr)
        delete FourierFilter;
    if (FourierFilt != nullptr)
        delete FourierFilt;
}
///
/// возвращение спектра генерируемого сигнала
void CSignal::getSignalSpectr(QVector<QPointF>& spectr)
{
    spectr.resize( K );
    for (size_t i = 0; i < K; i++)
        spectr[i] = QPointF(freq[i], Amp[i]);
}
///
/// генерация спектра сигнала
void CSignal::createSignalSpectr()
{
    srand((unsigned) time(0));
    for (size_t j = 0; j < K; j++)
    {
        if (boolRandomSpectr)
        {/// случайный спектр с произвольными амплитудами и частотами
            Amp[j] = QRandomGenerator::global()->bounded(MaxAmp);
//            freq[j] = dF*round(0.5*NSp*rand()/RAND_MAX);// частоты кратны dF
            freq[j] = QRandomGenerator::global()->bounded( 0.5*SamplingFreq );// частоты не обяз-но кратны dF
        }
        else
        {/// эквидистантный спектр с фиксированной амплитудой для всех компонент (частоты кратны dF)
            Amp[j] = MaxAmp;
            freq[j] = 0.5*SamplingFreq*((j+1.0) + posFirstSC)/(K+1);
        }
        w[j] = 2.0*M_PI*freq[j];
    }
    emit SpectrGenerated( );
}
///
/// возвращение амплитуды спектр. компоненты
double CSignal::getSpectrComp(double freq)
{
    for (size_t j = 0; j < K; j++)
    {
        if (this->freq[j] == freq)
            return this->Amp[j];
    }
    return 0.0;
}
///
void CSignal::timerEvent(QTimerEvent*)
{
    std::complex<double> SpectralComp[2];
    double meanFiltSC[2], FiltSC[2];
    double signal;//, pmeanFiltSC;
#ifdef BlockProcessing
    for (size_t i = 0; i < N; i++)
#endif
    {
        t += SamplingPeriod;
        /// вычисляем сигнал и помещаем его в циклический буфер
        /// входного сигнала
        signal = 0.0;
        for (size_t j = 0; j < K; j++)
            signal += Amp[j]*sin(w[j]*t);
        signal += relNoiseValue*MaxAmp*(2.0*rand()/RAND_MAX - 1.0);// шум
        /// обновляем индекс текущего - нового входного отсчёта сигнала
        SpectralComp[0] = FourierFilter->push( signal );
        SpectralComp[1] = FourierFilt->push( signal );
#ifdef BlockProcessing
        /// входной сигнал
        data[0].replace(i, QPointF(t, signal) );
        /// отфильтрованная спектральная компонента на детектированной амп-де
        data[1].replace(i, QPointF(t, SpectralComp[0].real() ) );
        /// отфильтрованная спектральная компонента на детектированной амп-де
        data[2].replace(i, QPointF(t, SpectralComp[1].real()) );
#else
        /// входной сигнал
        data[0].push_back( QPointF(t, signal) );
        data[0].pop_front();
        /// отфильтрованная спектральная компонента на детектированной амп-де
        data[1].push_back( QPointF(t, SpectralComp.real() ) );
        data[1].pop_front();
#endif
        /// амплитуда отфильтрованной спектральной компоненты
        index++;
        for (int i = 0; i < 2; i++)
        {
            FiltSC[i] = abs( SpectralComp[i] );
            sumFiltSpectralComp[i] += FiltSC[i];
            meanFiltSC[i] = sumFiltSpectralComp[i] / index;
            /// мгн. амплитуда фильтрованной ф.Г. компоненты
            SC[i].replace(0, QPointF( FourierFilter->getFiltFreq(), FiltSC[i]) );
            /// средняя амплитуда фильтрованной ф.Г. компоненты
            SC[i].replace(1, QPointF( FourierFilter->getFiltFreq(), meanFiltSC[i]) );
        }
        cout << index << ":\t|FiltSC| = " << FiltSC[1]
             << "\t|meanFiltSC| = " << meanFiltSC[1] << "\r";//<<abs(meanFiltSC - pmeanFiltSC)/meanFiltSC<<"\r";
    }
    emit DataUpdated();
//    emit SCDUpdated( meanFiltSC[1] );
    emit SCDUpdated( FiltSC[1] );
}
///
/// старт/стоп процесса генерации сигнала
void CSignal::onBtnPressed(bool state)
{
    if (state)
    {
        /// генерируем спектр сигнала
        createSignalSpectr();
        /// стартовая очистка сигнала и вспомог. переменных
        resetData();
        /// вывод параметров спектра сигнала
        cout<<"Signal spectr:\n   #\tfreq\tAmp"<<endl;
        for (size_t j = 0; j < K; j++)
            cout << "   " << j << ": " << setw(6)
                 << freq[j] << " [Hz]\t" << Amp[j]<<endl;
        /// старт вычислений
        idTimer = startTimer(TimerInterval);
    }
    else
    {
        killTimer( idTimer );
        cout << endl;
    }
    simulation = state;
}
///
/// очистка данных
void CSignal::resetData()
{
    /// отсутствие обнуления массива приводило к абс.
    /// неверному вычислению спектральных компонент (!)
    FourierFilter->reset();
    t = 0.0;
    for (size_t j = 0; j < N; j++)
        for (int k = 0; k < Nlines; k++)
        {
            t += SamplingPeriod;
            data[k].replace(j, QPointF(t, 0.0) );
        }
    index = 0;
    sumFiltSpectralComp[0] = sumFiltSpectralComp[1] = 0.0;
 }
///
/// число спектральных компонент в генерируемом сигнале
void CSignal::setK(int K)
{
    this->K = K;
    /// заново генерируем спектр сигнала
    createSignalSpectr();
    emit SpectrGenerated( );
    /// если запущен процесс, очищаем
    if (simulation)
        resetData();
}
///
/// положение первой спектральной компоненты в генерируемом сигнале
void CSignal::setPosFirstSC(double posFirstSC)
{
    this->posFirstSC = posFirstSC;
    /// заново генерируем спектр сигнала
    createSignalSpectr();
    emit SpectrGenerated( );
    /// если запущен процесс, очищаем
    if (simulation)
        resetData();
}
///
/// эквидистантный спектр с фиксированной амплитудой для всех компонент
void CSignal::setSignalAmp(double MaxAmp)
{
    this->MaxAmp = MaxAmp;
    for (size_t j = 0; j < K; j++)
        Amp[j] = MaxAmp;
    emit SpectrGenerated( );
    /// если запущен процесс, очищаем
    if (simulation)
        resetData();
}
///
/// спектр случайный или фиксированный
void CSignal::setRandomSignalSpectr(int boolRandomSpectr)
{
    this->boolRandomSpectr = boolRandomSpectr;
    createSignalSpectr();
    emit SpectrGenerated( );
    /// если запущен процесс, очищаем
    if (simulation)
        resetData();
}
///
/// относительная величина шума
void CSignal::setNoiseValue(double relNoiseValue)
{
    this->relNoiseValue = relNoiseValue;
}
