#ifndef CFOURIERFILT_H
#define CFOURIERFILT_H

#include <iostream>
#include <QVector>
#include <QPointF>
#include <complex>
#include <cmath>

//#define myvariant

class CFourierFilt
{
public:
    CFourierFilt(double SamplingFreq/*[Гц]*/, double FiltFreq/*[Гц]*/, double gT/*[r.u.]*/);
    ~CFourierFilt();
    /// вычисление текущего значения фильтра & сохранение текущего отсчёта в буфере
    double push(double sample);
    /// установка параметров Фурье-фильтра
    void setFourierFiltParam(double SamplingPeriod/*[с]*/, double FiltFreq/*[Гц]*/, double gT/*[r.u.]*/);
    /// получение частоты фильтрации
    double getFiltFreq() const
    {
        return FiltFreq;/*[Гц]*/
    }
    /// получение частоты дискретизации сигнала
    double getSamplingFreq() const
    {
        return SamplingFreq;/*[Гц]*/
    }
    /// вычисление ЧХ фильтра ( what = true/false -> spectr = АЧХ / ФЧХ )
    void getFrequencyResponse(QVector<QPointF>& spectr, bool what = true) const;
    /// сброс фильтра
    void reset();
    /// информация о фильтре
    void showFilterInfo(bool in_detail = true) const;

private:
    double FiltFreq;/**<[Гц] частота фильтрации */
    double SamplingFreq;/**<[Гц] частота дискретизации */
    double SamplingPeriod;/**<[с] период дискретизации */
    double SFC;/**< текущее значение Фурье-компоненты */
    double gT;
    double Relax;
    const std::complex<double> i = {0, 1};
#ifdef myvariant
    std::complex<double> q, pout;
#else
    double in[3];
    double out[3];
    double a[3];
    double b[3];
#endif
    int index;/**< индекс */
};

#endif // CFOURIERFILT_H
