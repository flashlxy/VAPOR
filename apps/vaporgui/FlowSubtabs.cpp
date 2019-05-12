#include "FlowSubtabs.h"

QVaporSubtab::QVaporSubtab(QWidget* parent) : QWidget(parent)
{
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0,0,0,0);
    _layout->insertSpacing(-1, 20);
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum);
}


//
//================================
//
FlowVariablesSubtab::FlowVariablesSubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _variablesWidget = new VariablesWidget(this);
    _variablesWidget->Reinit(   (VariableFlags)(VECTOR | COLOR),
                                (DimFlags)(THREED) );
    _layout->addWidget( _variablesWidget, 0, 0 );

    _velocityMltp = new QLineEdit( this );
    _layout->addWidget( _velocityMltp );

    _steady = new VCheckBox( this, "Use Steady Flow" );
    _layout->addWidget( _steady );

    _steadyNumOfSteps = new QLineEdit( this );
    _layout->addWidget( _steadyNumOfSteps);

    connect( _steady,           SIGNAL( _checkboxClicked() ), this, SLOT( _steadyGotClicked() ) );
    connect( _velocityMltp,     SIGNAL( editingFinished() ),  this, SLOT( _velocityMultiplierChanged() ) );
    connect( _steadyNumOfSteps, SIGNAL( editingFinished() ),  this, SLOT( _steadyNumOfStepsChanged() ) );
}

void 
FlowVariablesSubtab::Update( VAPoR::DataMgr      *dataMgr,
                             VAPoR::ParamsMgr    *paramsMgr,
                             VAPoR::RenderParams *rParams) 
{
    _params = dynamic_cast<VAPoR::FlowParams*>(rParams);
    assert(_params);
    _variablesWidget->Update(dataMgr, paramsMgr, rParams);

    // Update custom widgets
    bool isSteady = _params->GetIsSteady();
    _steady->SetCheckState( isSteady );

    auto mltp = _params->GetVelocityMultiplier();
    _velocityMltp->setText( QString::number( mltp, 'f', 3 ) );

    int numOfSteps = _params->GetSteadyNumOfSteps();
    _steadyNumOfSteps->setText( QString::number( numOfSteps ) );
}

void
FlowVariablesSubtab::_steadyGotClicked()
{
    bool userInput = _steady->GetCheckState();
    _params->SetIsSteady( userInput );
}

void
FlowVariablesSubtab::_velocityMultiplierChanged()
{
    bool ok;
    double d = _velocityMltp->text().toDouble( &ok );
    if( ok )    // Scott: this verification is no longer needed once the line edit has its own validator
        _params->SetVelocityMultiplier( d );
}

void 
FlowVariablesSubtab::_steadyNumOfStepsChanged()
{
    bool ok;
    int i = _steadyNumOfSteps->text().toInt( &ok );
    if( ok )    // Scott: this verification is no longer needed once the line edit has its own validator
        _params->SetSteadyNumOfSteps( i );
}

//
//================================
//
FlowAppearanceSubtab::FlowAppearanceSubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _TFWidget = new TFWidget(this);
    _TFWidget->Reinit((TFFlags)(SAMPLING | CONSTANT_COLOR));

    _layout->addWidget( _TFWidget, 0, 0 );

    _params = NULL;
}

void FlowAppearanceSubtab::Update(  VAPoR::DataMgr *dataMgr,
                                    VAPoR::ParamsMgr *paramsMgr,
                                    VAPoR::RenderParams *rParams) 
{
    _params = dynamic_cast<VAPoR::FlowParams*>(rParams);
    assert(_params);

    _TFWidget->Update(dataMgr, paramsMgr, rParams);
}


//
//================================
//
FlowSeedingSubtab::FlowSeedingSubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _geometryWidget   = new GeometryWidget(this);
    _geometryWidget->Reinit( 
        (DimFlags)THREED,
        (VariableFlags)VECTOR
    );
    _layout->addWidget( _geometryWidget );

    _seedGenMode = new VComboBox( this, "Seed Generation Mode" );
    /* Index numbers are in agreement with what's in FlowRenderer.h */
    _seedGenMode->AddOption( "Programatically", 0 );
    _seedGenMode->AddOption( "From a List", 1 );
    _layout->addWidget( _seedGenMode );
    connect( _seedGenMode, SIGNAL( _indexChanged(int) ), this, SLOT( _seedGenModeChanged(int) ) );
   
    _fileReader = new VFileReader( this, "Input Seed File" );
    _fileReader->SetFileFilter( QString::fromAscii("*.txt") );
    _layout->addWidget( _fileReader );
    connect( _fileReader, SIGNAL( _pathChanged() ), this, SLOT( _fileReaderChanged() ) );

    _flowDirection = new VComboBox( this, "Steady Flow Direction" );
    /* Index numbers are in agreement with what's in FlowRenderer.h */
    _flowDirection->AddOption( "Forward", 0 );
    _flowDirection->AddOption( "Backward", 1 );
    _flowDirection->AddOption( "Bi-Directional", 2 );
    _layout->addWidget( _flowDirection );
    connect( _flowDirection, SIGNAL(_indexChanged(int)), this, SLOT( _flowDirectionChanged(int) ) );

    _fileWriter = new VFileWriter( this, "Output Flow Lines" );
    _fileWriter->SetFileFilter( QString::fromAscii("*.txt") );
    _layout->addWidget( _fileWriter );
    connect( _fileWriter, SIGNAL( _pathChanged() ), this, SLOT( _fileWriterChanged() ) );

    _outputButton = new QPushButton( "Output Flow Lines", this );
    _layout->addWidget( _outputButton );
    connect( _outputButton, SIGNAL( clicked() ), this, SLOT( _outputButtonClicked() ) );
}

void
FlowSeedingSubtab::_outputButtonClicked( )
{
std::cerr << "button clicked" << std::endl;
    _params->SetNeedFlowlineOutput( true );
}

void FlowSeedingSubtab::Update( VAPoR::DataMgr      *dataMgr,
                                VAPoR::ParamsMgr    *paramsMgr,
                                VAPoR::RenderParams *params )
{
    _params = dynamic_cast<VAPoR::FlowParams*>(params);

    //VAPoR::Box* rakeBox = params->GetRakeBox();
    //_geometryWidget->Update(paramsMgr, dataMgr, params, rakeBox);
    _geometryWidget->Update(paramsMgr, dataMgr, params );

    long idx = _params->GetSeedGenMode();
    if( idx >= 0 && idx < _seedGenMode->GetNumOfItems() )
        _seedGenMode->SetIndex( idx );
    else
        _seedGenMode->SetIndex( 0 );

    if( !_params->GetSeedInputFilename().empty() ) 
        _fileReader->SetPath( _params->GetSeedInputFilename() );
    if( !_params->GetFlowlineOutputFilename().empty() ) 
        _fileWriter->SetPath( _params->GetFlowlineOutputFilename() );
}

void
FlowSeedingSubtab::_seedGenModeChanged( int newIdx )
{
    _params->SetSeedGenMode( newIdx );
}

void
FlowSeedingSubtab::_fileReaderChanged()
{
    std::string filename = _fileReader->GetPath();
    _params->SetSeedInputFilename( filename );
}

void
FlowSeedingSubtab::_fileWriterChanged()
{
    std::string filename = _fileWriter->GetPath();
    _params->SetFlowlineOutputFilename( filename );
}

void
FlowSeedingSubtab::_flowDirectionChanged( int newIdx )
{
    _params->SetFlowDirection( newIdx );
}


//
//================================
//
FlowGeometrySubtab::FlowGeometrySubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _geometryWidget   = new GeometryWidget(this);
    _copyRegionWidget = new CopyRegionWidget(this);
    _transformTable   = new TransformTable(this);
    _geometryWidget->Reinit( 
        (DimFlags)THREED,
        (VariableFlags)VECTOR
    );

    _layout->addWidget( _geometryWidget, 0 ,0 );
    _layout->addWidget( _copyRegionWidget, 0 ,0 );
    _layout->addWidget( _transformTable, 0 ,0 );

    _params = NULL;
}

void FlowGeometrySubtab::Update( VAPoR::ParamsMgr *paramsMgr,
                                 VAPoR::DataMgr *dataMgr,
                                 VAPoR::RenderParams *rParams) 
{
    _params = dynamic_cast<VAPoR::FlowParams*>(rParams);
    assert(_params);

    _geometryWidget->Update(paramsMgr, dataMgr, rParams);
    _copyRegionWidget->Update(paramsMgr, rParams);
    _transformTable->Update(rParams->GetTransform());
}


//
//================================
//
FlowAnnotationSubtab::FlowAnnotationSubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _colorbarWidget = new ColorbarWidget(this);
    _layout->addWidget(_colorbarWidget, 0, 0);
}

void FlowAnnotationSubtab::Update(  VAPoR::ParamsMgr *paramsMgr,
                                    VAPoR::DataMgr *dataMgr,
                                    VAPoR::RenderParams *rParams) 
{
    _colorbarWidget->Update(dataMgr, paramsMgr, rParams);
}
