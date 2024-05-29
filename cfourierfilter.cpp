#include "cfourierfilter.h"
#include <cmath>
///
///
CFourierFilter::CFourierFilter(int length)
{
    this->length = length;
    in = new double[length];
    Norma = 2.0/length;
}

CFourierFilter::CFourierFilter(int length, double SamplingFreq, double FiltFreq)
{
    this->length = length;
    in = new double[length];
    setFourierFiltParam( SamplingFreq, FiltFreq );
    Norma = 2.0/length;
}
///
///
CFourierFilter::~CFourierFilter()
{
    delete [] in;
    in = nullptr;
}
///
///
void CFourierFilter::setFourierFiltParam(double SamplingFreq/*[Гц]*/, double FiltFreq/*[Гц]*/)
{
    this->FiltFreq = FiltFreq;
    this->SamplingFreq = SamplingFreq;
    std::complex<double> i(0, 1);

    SamplingPeriod = 1.0/SamplingFreq;//[с]
    dF = SamplingFreq/length;
    q = exp(i*2.0*M_PI*FiltFreq/SamplingFreq);
    qq = exp(i*2.0*M_PI*FiltFreq/SamplingFreq - 0.01);
    std::cout << "FiltFreq = " << FiltFreq << " [Hz]"
              << "\nFiltFreq/dF = " << FiltFreq / dF
              << "\nфазовый множитель: q = " << q
              << "\nконтроль: q^N = (!) 1.0 + 0.0*i (!) =" << exp(i*2.0*M_PI*FiltFreq/dF) << std::endl;
    /// сброс содержимого циклического буфера
    reset();
}
///
/// сброс содержимого циклического буфера
void CFourierFilter::reset()
{
    pSFC = 0.0;
    cindex = -1;
    for (int i = 0; i < length; i++)
        in[i] = 0;
}
///
/// сохранение отсчёта входного сигнала в буфер и фильтрация
std::complex<double> CFourierFilter::push(double sample)
{
    cindex = (cindex < length-1) ? cindex+1 : 0;
//    SFC = q*(pSFC + sample - in[cindex]);// без затухания (с буфером)
    SFC = q*pSFC + sample - in[cindex];// без затухания (с буфером)
//    SFC = qq*pSFC + sample;// с затуханием (без буфера)
    in[cindex] = sample;
    pSFC = SFC;
    ///   амплитуда = Norma*abs(SFC)
    ///   отфильтрованный сигнал = Norma*SFC.real()
    return Norma*SFC;//отфильтрованный сигнал
}
///
/// вычисление ЧХ одночастотного фильтра
/// what = true/false (spectr = АЧХ/ФЧХ)
void CFourierFilter::getFrequencyResponse(QVector<QPointF>& spectr, bool what) const
{
    if (spectr.size() == 0)
        spectr.resize( 8*length );
    double f = 0.0, df = .5*SamplingFreq/spectr.size(), absH;
    std::complex<double> i(0, 1), z, zN, H;
    for (int j = 0; j < spectr.size(); j++)
    {
        z = exp(-i*2.0*M_PI*f/SamplingFreq);
        zN = exp(-i*2.0*M_PI*f/dF);
        H = 2.0/length*q*(1.0 - zN)/(1.0 - q*z);
        absH = abs(H);
        if (absH == 0 || std::isnan(absH))
            absH = 1e-18;
        spectr[j] = QPointF(f, (what) ? absH : std::arg(H)/M_PI);
        f += df;
    }
    /// коррекция АЧХ
    if (what)
        for (int j = 1; j < spectr.size()-1; j++)
            if (std::isinf(spectr[j].y()))
            {
                std::cout << "коррекция АЧХ  в пике:" << spectr[j].ry();
                spectr[j].setY( 0.5*(spectr[j-1].y() + spectr[j+1].y()) );
                std::cout << "  ->  " << spectr[j].ry() << std::endl;
                /// должно быть так : (!)
//                spectr[j].setY( sqrt(2.0/length*spectr.size()) );
//                spectr[j].setY( sqrt(2.0) );
            }
}
///
/// информация о фильтре
void CFourierFilter::showFilterInfo(bool in_detail) const
{
    if (!in_detail)
    {
        std::cout << "частота фильтрации Фурье-фмльтра (СFourierFilter):\t"
                    << FiltFreq << " [Гц]" << std::endl;
        return;
    }
    std::cout << "  Фурье-фмльтр (СFourierFilter): "
              << "\n    частота дискретизации:\t" << SamplingFreq << " [Гц]"
              << "\n    период дискретизации:\t" << SamplingPeriod << " [c]"
              << "\n    частота фильтрации:\t" << FiltFreq << " [Гц]"
              << "\n    индекс фильтруемой частоты:\t" << FiltFreq / (SamplingFreq/length)
              << "\n    шаг по частоте:\t" << SamplingFreq/length << " [Гц]"
              << "\n    длина буфера данных:\t" << length
              << "\n    фазовый множитель:\t" << q << std::endl;
}
///
/// для контроля
void CFourierFilter::showBuff()
{
    int counter = -1, index = cindex;
    while (counter < 5)
    {
        index = (index < length-1) ? index+1 : 0;
        std::cout << in[index] << "\t";
        counter++;
    }
    std::cout << std::endl;

}
