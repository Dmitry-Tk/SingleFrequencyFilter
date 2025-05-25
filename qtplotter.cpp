#include "qtplotter.h"
#include "csignal.h"
#include <QDebug>
#include <QString>
#include <math.h>
///
///
QtPlotter::QtPlotter(QWidget *parent, TData type) :  QChartView(parent)
{
    this->type = type;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setRubberBand(QChartView::RectangleRubberBand);
    setChart( &chart );
    chart.setTheme( QChart::ChartTheme::ChartThemeDark );
    chart.legend()->hide();
    boolAutoScale = boolLogAxisY = false;
}
///
///
void QtPlotter::CreateAxesAndSeries()
{
    QList<QAbstractAxis*>  HorizAxes, VertAxes;
    QString lblX, lblY;
    QPen* pen;
    pen = new QPen();
    pen->setWidth(2);
    switch (type)
    {
    case Sig:
        for (int j = 0; j < Nlines; j++)
        {
            line[j] = new QLineSeries();
            chart.addSeries( line[j] );
        }
        pen->setColor( Qt::green );
        line[0]->setPen( *pen );
        pen->setColor( Qt::red );
        line[1]->setPen( *pen );
        pen->setColor( Qt::white );
        line[2]->setPen( *pen );
        lblX = "time, [s]";
        lblY = "Signal";
        break;
    case Spectr:
        LogValueAxisY = nullptr;
        RealSpectr.setColor( Qt:: green );
        RealSpectr.append(0.1, 0);
        RealSpectr.setMarkerShape( QScatterSeries::MarkerShapeRectangle );
        RealSpectr.setMarkerSize( 6 );
        RealSpectr.setBorderColor( RealSpectr.color() );
        chart.addSeries( &RealSpectr );
        /// добавляем спектры одночастотных фильтров
        FilterSpectr[0].setPen( *pen );
        FilterSpectr[0].append(0.1, 0);
        FilterSpectr[0].setColor( Qt::yellow );
        chart.addSeries( &FilterSpectr[0] );
        FilterSpectr[1].setPen( *pen );
        FilterSpectr[1].append(0.1, 0);
        FilterSpectr[1].setColor( Qt::white);
        chart.addSeries( &FilterSpectr[1] );
        /// добавляем фильтруемую спектральную компоненту
        for (int j = 0; j < 2; j++)
        {
            detectedSC[j].setMarkerShape((j ==0) ? QScatterSeries::MarkerShapeCircle : QScatterSeries::MarkerShapeRectangle );
            detectedSC[j].setMarkerSize( 6 );
            detectedSC[j].setColor( (j == 0) ? Qt::red : Qt::white );
            detectedSC[j].setBorderColor( detectedSC[j].color() );
            detectedSC[j].append(0.1, 0);
            chart.addSeries( &detectedSC[j] );
        }
        lblX = "frequency, [Hz]";
        lblY = "Spectr";
    }
    delete pen;
    chart.createDefaultAxes();
    HorizAxes = chart.axes(Qt::Horizontal);
    VertAxes = chart.axes(Qt::Vertical);
    axisX = HorizAxes[0];
    axisY = VertAxes[0];
    axisX->setTitleText( lblX );
    axisY->setTitleText( lblY );
    switch (type)
    {
    case Sig:
//        axisX->setRange( 0.0, 0.5*Signal->getSamplingFreq() );
//        axisY->setRange( -1.5*Signal->getMaxAmp(), 1.5*Signal->getMaxAmp() );
        break;
    case Spectr:
        ValueAxisY = static_cast<QValueAxis*>(axisY);
    }
    /// контроль осей координат
//    QList<QAbstractSeries *> SerList = chart.series();
//    for (int i = 0; i < chart.series().size(); i++)
//    {
//        qDebug()<<i<<":"<<chart.series()[i];
//        QList<QAbstractAxis*> attachedAxes = SerList[i]->attachedAxes();
//        for (int j = 0; j < attachedAxes.size(); j++)
//            qDebug()<<"\t"<<attachedAxes[j];
//    }
}
///
///
double getMax(const QVector<QPointF>& v)
{
    double max = v.at(0).y();
    for(auto& point : v)
        if (max < point.y())
            max = point.y();

    return max;
}
///
/// автомасштабирование спектров
void QtPlotter::setAutoScale(int boolAutoScale)
{
    this->boolAutoScale = (bool)boolAutoScale;
    if (type == Spectr && boolAutoScale)
    {
        axisX->setRange( 0.0, 0.5*Signal->getSamplingFreq());
        if (boolLogAxisY)
            axisY->setRange( 1e-6, 100.0);
        else
            axisY->setRange( 0.0, 1.25*std::max(Signal->getMaxAmp(), MaxFilterSpectrValue) );
    }
}
///
/// замена оси Y обычной на логариф. и наоборот
void QtPlotter::setNewYAxis(int boolLogAxisY)
{
    this->boolLogAxisY = (bool)boolLogAxisY;
    /// открепляем ось от кривых
    for (auto& ser: chart.series())
        ser->detachAxis( axisY );
    axisY->setVisible( false );
    /// меняем ось
    if (boolLogAxisY)
    {
        if (LogValueAxisY == nullptr)
        {
            LogValueAxisY = new QLogValueAxis();
            LogValueAxisY->setBase(10.0);
            LogValueAxisY->setLabelFormat("%g");
            LogValueAxisY->setMinorTickCount(-1);
            chart.addAxis(LogValueAxisY, Qt::AlignLeft);
            axisX->setTitleText("frequency, [Hz]");
            axisY->setTitleText("Spectr");
        }
        axisY = LogValueAxisY;
    }
    else
        axisY = ValueAxisY;
    /// прикрепляем ось к кривым
    for (auto& ser: chart.series())
        ser->attachAxis( axisY );
    axisY->setVisible( true );
    /// диапазон
//    if (boolLogAxisY)
//        axisY->setRange( 1e-3, 100.0);
//    else
//        axisY->setRange( 0.0, 1.5*Signal->getMaxAmp() );
}
///
/// обновление графика сигналов
void QtPlotter::updateSignalChart()
{
    if (type == Sig)
    {

        line[0]->replace( Signal->data[0] );
        line[1]->replace( Signal->data[1] );
        line[2]->replace( Signal->data[2] );
        size_t N = Signal->data[0].size();
        axisX->setRange( Signal->data[0].data()[0].rx(), Signal->data[0].data()[N-1].rx());
        if (boolAutoScale)
            axisY->setRange( -3.0*Signal->getMaxAmp(), 3.0*Signal->getMaxAmp() );
    }
    else/// type == Spectr
    {
        detectedSC[0].replace( Signal->SC[0] );
        detectedSC[1].replace( Signal->SC[1] );
        if (boolAutoScale)
        {
            axisX->setRange( 0.0, 0.5*Signal->getSamplingFreq());
            if (boolLogAxisY)
                axisY->setRange( 1e-6, 100.0);
            else
                axisY->setRange( 0.0, 1.25*std::max(Signal->getMaxAmp(), MaxFilterSpectrValue) );
        }
    }
}
///
/// обновление графика спектра сигнала и ЧХ фильтра
void QtPlotter::onFiltFreqUpdated(double FiltFreq)
{
    chart.setTitle( QString::number( FiltFreq ) + " [Hz]" );
    onSpectrUpdated();
}
///
///
void QtPlotter::onSpectrUpdated()
{
//    if (type == Spectr)
    {
        /// добавляем график спектра сигнала
        QVector<QPointF> spectr;
        Signal->getSignalSpectr( spectr );
        RealSpectr.replace( spectr );
        /// добавляем график спектра одночастотного фильтра
        QVector<QPointF> filterSpectr;
        Signal->FourierFilter->getFrequencyResponse(filterSpectr, what);
        MaxFilterSpectrValue = getMax( filterSpectr);
        if (boolLogAxisY)
            filterSpectr[0] = filterSpectr[1];// исключаем freq = 0.0, если лог масштаб
        FilterSpectr[0].replace( filterSpectr );

//        filterSpectr.clear();
        Signal->FourierFilt->getFrequencyResponse(filterSpectr, what);
        if (boolLogAxisY)
            filterSpectr[0] = filterSpectr[1];// исключаем freq = 0.0, если лог масштаб
        FilterSpectr[1].replace( filterSpectr );

        if (boolAutoScale)
        {
//            axisX->setRange( filterSpectr.front().rx(), filterSpectr.back().rx() );
            axisX->setRange( 0.0, 0.5*Signal->getSamplingFreq() );
            axisY->setRange( 0.0, 1.25*std::max(Signal->getMaxAmp(), MaxFilterSpectrValue) );
        }
    }
}
