#ifndef CFOURIERFILTER_H
#define CFOURIERFILTER_H

#include <iostream>
#include <complex>
#include <QVector>
#include <QPointF>

class CFourierFilter
{
public:
    CFourierFilter(int length);
    CFourierFilter(int length, double SamplingFreq/*[Гц]*/, double FiltFreq/*[Гц]*/);
    ~CFourierFilter();
    /// вычисление текущего значения фильтра & сохранение текущего отсчёта в буфере
    std::complex<double> push(double sample);
    /// установка параметров Фурье-фильтра
    void setFourierFiltParam(double SamplingPeriod/*[с]*/, double FiltFreq/*[Гц]*/);
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
    ///
    void showBuff();

private:
    double FiltFreq;/**<[Гц] частота фильтрации */
    double SamplingFreq;/**<[Гц] частота дискретизации */
    double SamplingPeriod;/**<[с] период дискретизации */
    double dF;/**< шаг по частоте */
    std::complex<double> q, qq;/**< Фурье-множитель */
    std::complex<double> SFC;/**< текущее значение Фурье-компоненты */
    std::complex<double> pSFC;/**< предыдущее значение Фурье-компоненты */
    double* in;/**< циклический буфер */
    int length;/**< длина циклического буфера */
    int cindex;/**< циклический индекс */
    double Norma;
};

#endif // CFOURIERFILTER_H
