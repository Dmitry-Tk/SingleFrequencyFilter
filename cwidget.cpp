#include "cwidget.h"
#include <QDebug>

CWidget::CWidget()
{
    QVBoxLayout* vbox;
    QHBoxLayout* hbox;
    QLabel* lbl;
    QHBoxLayout* Hbox = new QHBoxLayout;
    /// управление - левая панель
    btnpause = new QPushButton( "Start&&Stop");
    chboxLogY = new QCheckBox("log10(Y)");
    chboxAutoScale = new QCheckBox("auto scale");
    spboxNoise = new QDoubleSpinBox();
    vbox = new QVBoxLayout;
    hbox = new QHBoxLayout;
    vbox->addWidget( btnpause );
    lbl = new QLabel( "noise" );
    hbox->addWidget( lbl );
    hbox->addWidget( spboxNoise );
    vbox->addLayout( hbox );
    vbox->addWidget( chboxLogY );
    vbox->addWidget( chboxAutoScale );
    Hbox->addLayout( vbox );
    /// параметры фильтра - центральная
    spboxNSp = new QSpinBox();
    sliderFC = new QSlider(Qt::Horizontal);
    spboxShiftFC = new QDoubleSpinBox();
    QGroupBox* grboxFilter = new QGroupBox( "Filter parameters" );
    vbox = new QVBoxLayout;
    hbox = new QHBoxLayout;
    lbl = new QLabel( "number of spectral components" );
    hbox->addWidget( lbl );
    hbox->addWidget( spboxNSp );
    vbox->addLayout( hbox );
    hbox = new QHBoxLayout;
    lbl = new QLabel( "position" );
    hbox->addWidget( lbl );
    hbox->addWidget( sliderFC );
    vbox->addLayout( hbox );
    hbox = new QHBoxLayout;
    lbl = new QLabel( "displacement" );
    hbox->addWidget( lbl );
    hbox->addWidget( spboxShiftFC );
    vbox->addLayout( hbox );
    grboxFilter->setLayout( vbox );
    Hbox->addWidget( grboxFilter );
    /// параметры спектра сигнала - правая
    spboxK = new QSpinBox();
    spboxPosFirstSC = new QDoubleSpinBox();
    spboxSignalAmp = new QDoubleSpinBox();
    QGroupBox* grboxSignalSpectr = new QGroupBox( "Signal spectr" );
    vbox = new QVBoxLayout;
    hbox = new QHBoxLayout;
    lbl = new QLabel( "number of components in Signal" );
    hbox->addWidget( lbl );
    hbox->addWidget( spboxK );
    vbox->addLayout( hbox );
    hbox = new QHBoxLayout;
    lbl = new QLabel( "first component position" );
    hbox->addWidget( lbl );
    hbox->addWidget( spboxPosFirstSC );
    vbox->addLayout( hbox );
    hbox = new QHBoxLayout;
    lbl = new QLabel( "amp of components in Signal" );
    hbox->addWidget( lbl );
    hbox->addWidget( spboxSignalAmp );
    vbox->addLayout( hbox );
    chboxRandomSpectr = new QCheckBox("random spectr");
    vbox->addWidget( chboxRandomSpectr );
    grboxSignalSpectr->setLayout( vbox );
    Hbox->addWidget( grboxSignalSpectr );
    SpectraData.setColumnCount( 2 );
    SpectraData.setRowCount( 3 );
    SpectraData.setHorizontalHeaderLabels( QStringList({"Amplitude [r.u.]", "Frequency [Hz]"}) );
    /// подстраиваем ширины колонок под содержимое
    SpectraData.horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    /// последняя колонка растягиваемая до конца таблицы
    SpectraData.horizontalHeader()->setStretchLastSection(true);
    SpectraData.verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    SpectraData.setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
    Hbox->addWidget( &SpectraData );
    /// метка для детектированного сигнала
    QFont font = QFont("Times New Roman", 14);
    lblSpectrComp = new QLabel();
    lblSpectrComp->setFont(font);
    lblSpectrCompDetected = new QLabel();
    lblSpectrCompDetected ->setFont(font);
    hbox = new QHBoxLayout;
    hbox->addWidget( new QLabel("Spectral Component: ") );
    hbox->addWidget( lblSpectrComp );
    hbox->addWidget( new QLabel("Spectral Component Detected: ") );
    hbox->addWidget( lblSpectrCompDetected );

    QVBoxLayout* Vbox = new QVBoxLayout;
    Vbox->addLayout( Hbox );
    Vbox->addLayout( hbox );
    setLayout( Vbox );
    /// задаём свойства виджетов управления
    btnpause->setCheckable( true );
    btnpause->resize(100, 30);
    spboxNoise->setRange(0.0, 1.0);
    spboxNoise->setSingleStep(0.001);
    spboxNoise->setDecimals( 3 );
    spboxNoise->setValue( 0.0 );
    /// свойства фильтра
    spboxNSp->setRange(16, 4096);
    spboxShiftFC->setRange(-1.0, 1.0);
    spboxShiftFC->setSingleStep(0.001);
    spboxShiftFC->setDecimals( 3 );
    spboxShiftFC->setValue( 0.0 );
    sliderFC->setMinimum( 0 );
    /// свойства спектра сигнала
    spboxK->setRange( 1, 6);
    spboxPosFirstSC->setRange(-1.0, 1.0);
    spboxPosFirstSC->setSingleStep(0.01);
    spboxSignalAmp->setRange(0.01, 5.0);
    spboxSignalAmp->setSingleStep(0.01);
    ///
    lblSpectrComp->setToolTip( "Spectral component" );
    lblSpectrCompDetected->setToolTip( "Spectral component detected" );
    ///
    /// создаём фильтр и передаём его указатель в сигнал
    ///
    Signal = new CSignal(128, SamplingFreq, 4, 100);
    double dF = SamplingFreq / spboxNSp->value();
    double FiltFreq = (sliderFC->value() + spboxShiftFC->value())*dF;
    Signal->FourierFilter = new CFourierFilter( spboxNSp->value(), SamplingFreq, FiltFreq );
    Signal->FourierFilt = new CFourierFilt( SamplingFreq, FiltFreq, gT );
    Signal->setPosFirstSC( spboxPosFirstSC->value() );
    ///
    /// CtrlWgt -> Signal
    connect( btnpause, SIGNAL(clicked(bool)), Signal, SLOT(onBtnPressed(bool)) );
    connect( spboxNoise, SIGNAL(valueChanged(double)), Signal, SLOT(setNoiseValue(double)) );
    /// управление ЧХ фильтра
    connect( spboxNSp, SIGNAL(valueChanged(int)), this, SLOT(setSliderMaximum(int)) );
    connect( sliderFC, SIGNAL(valueChanged(int)), this, SLOT(setToolTip(int)) );
    connect( spboxNSp, SIGNAL(valueChanged(int)), this, SLOT(setNSp(int)) );
    connect( sliderFC, SIGNAL(valueChanged(int)), this, SLOT(setFiltFreq()) );
    connect( spboxShiftFC, SIGNAL(valueChanged(double)), this, SLOT(setFiltFreq()) );
    /// управление спектром сигнала
    connect( spboxK, SIGNAL(valueChanged(int)), Signal, SLOT(setK(int)) );
    connect( spboxPosFirstSC, SIGNAL(valueChanged(double)), Signal, SLOT(setPosFirstSC(double)) );
    connect( spboxSignalAmp, SIGNAL(valueChanged(double)), Signal, SLOT(setSignalAmp(double)) );
    connect( chboxRandomSpectr, SIGNAL(stateChanged(int)), Signal, SLOT(setRandomSignalSpectr(int)) );
    connect( Signal, SIGNAL(SpectrGenerated()), this, SLOT(onSpectrUpdated()) );
}
///
///
CWidget::~CWidget()
{

}
///
///
void CWidget::setToolTip(int newVal)
{
    sliderFC->setToolTip( QString::number(newVal) );
}
///
/// установка нового максимального значения слайдера индекса фильтруемой частоты
void CWidget::setSliderMaximum(int newVal)
{
    sliderFC->setMaximum( (int)(newVal/2) );
    sliderFC->setToolTip( QString::number(sliderFC->value()) );
}
/// реакция на изменение размера циклического буфера входного сигнала
/// и соот-но длины вектора Фурье-преобразования -> создание нового фильтра
void CWidget::setNSp(int NSp)
{
    double dF = SamplingFreq / NSp;
    double FiltFreq = (sliderFC->value() + spboxShiftFC->value())*dF;

    delete Signal->FourierFilter;
    Signal->FourierFilter = new CFourierFilter( NSp, SamplingFreq, FiltFreq );

    delete Signal->FourierFilt;
    Signal->FourierFilt = new CFourierFilt( SamplingFreq, FiltFreq, gT );
}
///
/// установка новой частоты фильтрации
void CWidget::setFiltFreq()
{
    double dF = SamplingFreq / spboxNSp->value();
    double FiltFreq = (sliderFC->value() + spboxShiftFC->value())*dF;

    Signal->FourierFilter->setFourierFiltParam( SamplingFreq, FiltFreq );
    Signal->FourierFilt->setFourierFiltParam( SamplingFreq, FiltFreq, gT );
    /// обновляем значение частоты фильтрации
    lblSpectrComp->setText( QString::number( FiltFreq ) + tr(" [Hz]") );
    /// сбрасываем данные в фильтрах
    if (Signal->simulation)
        Signal->resetData();
    emit FiltFreqUpdated( FiltFreq );
}
///
/// обновляем таблицу спектральных данных
void CWidget::onSpectrUpdated()
{
    QTableWidgetItem* item;
    if (SpectraData.rowCount() != static_cast<int>(Signal->K))
        SpectraData.setRowCount( Signal->K );
    for(size_t row = 0; row < Signal->K; row++)
    {
//        item->setBackground( seriesColor );
//        item = new QTableWidgetItem( QString("%1").arg( .. ) );
        item = new QTableWidgetItem( QString::number( Signal->Amp[row] ) );
        item->setFlags( item->flags() & ~Qt::ItemIsEditable );
        SpectraData.setItem( row, 0,  item );
        item = new QTableWidgetItem( QString::number( Signal->freq[row] ) );
        item->setFlags( item->flags() & ~Qt::ItemIsEditable );
        SpectraData.setItem( row, 1,  item );
    }

}
///
/// отображение детектированной текущей спектральной компоненты
void CWidget::updateSpectrCompDetectValue(double value)
{
    lblSpectrCompDetected->setText( QString::number(value, 'f', 4) );
}
