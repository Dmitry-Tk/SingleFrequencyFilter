#include "cfourierfilt.h"
///
///
CFourierFilt::CFourierFilt(double SamplingFreq/*[Гц]*/, double FiltFreq/*[Гц]*/, double gT/*[r.u.]*/)
{
    setFourierFiltParam( SamplingFreq, FiltFreq, gT );
}
///
///
CFourierFilt::~CFourierFilt()
{
}
///
/// H = (a[0] + a[1]*cos(2*pi*fk)*z.^-1 + a[2]*z.^-2))./(b[0] + b[1]*cos(2*pi*fk)*z.^-1 + b[2]*z.^-2);
void CFourierFilt::setFourierFiltParam(double SamplingFreq/*[Гц]*/, double FiltFreq/*[Гц]*/, double gT/*[r.u.]*/)
{
    this->FiltFreq = FiltFreq;
    this->SamplingFreq = SamplingFreq;
    SamplingPeriod = 1.0/SamplingFreq;//[с]
    this->gT = gT;

    std::cout << "Фильтр с затуханием (без буфера):\n"
              << "   FiltFreq = " << FiltFreq/SamplingFreq << " [SamplingFreq]\n"
              << "   g*T = " << gT << " [r.u.]" << std::endl;
    Relax = exp(-gT);
#ifdef myvariant
    /// I
    q = Relax*exp(i*2.0*M_PI*FiltFreq/SamplingFreq);
    std::cout << "  q = " << q/Relax << "  R = " << abs(Relax) << std::endl;
#else
    /// II
    double Cos = cos(2.0*M_PI*FiltFreq/SamplingFreq);
    double Bw = (1.0 - Relax)/3.0;// полуширина фильтра
    double R = 1.0 - 3.0*Bw;// параметр затухания
    double K = (1.0 - 2.0*R*Cos + R*R)/(2.0 - 2.0*Cos);
    /// одночастотный пропускающий
    a[0] = 1.0 - K;
    a[1] = 2.0*(K-R)*Cos;
    a[2] = R*R - K;
    /// одночастотный режекторный
//    a[0] = K;
//    a[1] =-2.0*K*Cos;
//    a[2] = K;
    b[0] = 1.0;
    b[1] =-2.0*R*Cos;
    b[2] = R*R;
#endif
    /// сброс содержимого буфера
    reset();
}
///
/// сброс содержимого буфера
void CFourierFilt::reset()
{
    index = -1;
#ifdef myvariant
    pout = {0, 0};
#else
    in[0] = in[1] = in[2] = out[0] = out[1] = out[2] = 0;
#endif
}
///
/// сохранение отсчёта входного сигнала в буфер и фильтрация
double CFourierFilt::push(double sample)
{
    index++;
#ifdef myvariant
    std::complex<double> out = q*pout + (1.0-Relax)*sample;
    pout = out;
//    return abs(out);//отфильтрованный сигнал
    return out.real();//отфильтрованный сигнал
#else
    in[2] = in[1];
    in[1] = in[0];
    in[0] = sample;
    out[2] = out[1];
    out[1] = out[0];
    out[0] = -b[1]*out[1] - b[2]*out[2] + a[0]*in[0] + a[1]*in[1] + a[2]*in[2];

    return out[0];//отфильтрованный сигнал
#endif
}
///
/// вычисление ЧХ одночастотного фильтра
/// what = true/false (spectr = АЧХ/ФЧХ)
/// H = (a[0] + a[1]*cos(2*pi*fk)*z.^-1 + a[2]*z.^-2))./(b[0] + b[1]*cos(2*pi*fk)*z.^-1 + b[2]*z.^-2);
void CFourierFilt::getFrequencyResponse(QVector<QPointF>& spectr, bool what) const
{
    if (spectr.size() == 0)
        spectr.resize( 200 );
    double f = 0.0, df = 0.5*SamplingFreq/spectr.size(), absH;
    std::complex<double> z, H;
    for (int j = 0; j < spectr.size(); j++)
    {
        z = exp(-i*2.0*M_PI*f/SamplingFreq);
#ifdef myvariant
        H = (1.0 - Relax)/(1.0 - q*z);
//        double Cos = cos(2.0*M_PI*(f-FiltFreq)/SamplingFreq);
//        double Sin = sin(2.0*M_PI*(f-FiltFreq)/SamplingFreq);
//        H = (1.0 - Relax)/sqrt((1.0 - Relax*Cos)*(1.0 - Relax*Cos) + R*Sin*R*Sin);
#else
        H = (a[0] + a[1]*z + a[2]*z*z)/(b[0] + b[1]*z + b[2]*z*z);
#endif
        absH = abs(H);
        if (absH == 0 || std::isnan(absH))
            absH = 1e-18;
        spectr[j] = QPointF(f, (what) ? absH : std::arg(H)/M_PI);
        f += df;
    }
}
///
/// информация о фильтре
void CFourierFilt::showFilterInfo(bool in_detail) const
{
    if (!in_detail)
    {
        std::cout << "частота фильтрации Фурье-фмльтра (СFourierFilter):\t"
                    << FiltFreq << " [Гц]" << std::endl;
        return;
    }
    std::cout << "  Фурье-фмльтр (СFourierFilt): "
              << "\n    частота дискретизации:\t" << SamplingFreq << " [Гц]"
              << "\n    период дискретизации:\t" << SamplingPeriod << " [c]"
              << "\n    частота фильтрации:\t" << FiltFreq << " [Гц]"
              << "\n    параметр затухания:\t" << gT << " [отн.ед.]" << std::endl;
}
