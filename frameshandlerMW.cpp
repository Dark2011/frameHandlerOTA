//own
#include "frameshandlerMW.h"
#include "framesHandlerKernel.h"
#include "framesHandlerKernel_IFace.h"
#include "pixmapLabel.h"
//qt
#include <QtGui/QAction>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtCore/QString>
#include <QtGui/QFileDialog>
#include <QtGui/QIcon>
#include <QtGui/QLayout>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtCore/QDebug>
#include <QtGui/QToolBar>
#include <QtGui/QLineEdit>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QActionGroup>
#include <QtGui/QCheckBox>

//test
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QContextMenuEvent>

//new test
#include "FLSAlgorithm.h"


FrameHandlerMWindow::FrameHandlerMWindow(QWidget *parent) : QMainWindow(parent), 
m_framesHandler( new FramesHandlerKernel(this) ), m_startVideoState(false), algCreator(), 
frameHandleAlgorithm( algCreator.create(FAC::ONE_FRAME_TYPE2) ), m_videoMode(false),
m_cameraMode(false), wasSaved(false), m_startHandle(false)
{
	createActions();
	createMenus();
	createVideoPathLE();
	createToolBarServWgt();
	createFilters();
	createPixmapLabel();
	createResultWgt();
	createSlider();
	createButtons();
	createFramesCounterWgts();
	createBrightnWgts();
	createViewLayout();
	createControlLayout();
	createMainWgt();

	addToolBar( Qt::TopToolBarArea, createToolBar() );

	connect( m_framesHandler, SIGNAL( setImage(const QImage&) ), this, SLOT( setImage(const QImage&) ) );
	connect( m_framesHandler, SIGNAL( finished() ), this, SLOT( stopPlaying() ) );
	connect( m_framesHandler, SIGNAL( setAllFrames(int) ), this, SLOT( setAllFrames(int) ) );
	connect( m_framesHandler, SIGNAL( setCurrentMovePos(int) ), this, SLOT( moveSliderSlot(int) ) );

	connect( frameHandleAlgorithm.data(), SIGNAL( setFrameProperties(int, int) ),
			 this, SLOT( setFrameProperties(int, int) ) );
	connect( frameHandleAlgorithm.data(), SIGNAL( sendResults(const HandleResult &) ),
			 this, SLOT( writeResultToLog(const HandleResult &) ) );

	setCentralWidget(mainWidget);
	setMinimumSize(1100, 768);
	setMaximumSize(1100, 768);
}

void FrameHandlerMWindow::createActions()
{
	openVideoAction = new QAction( QIcon(":/FramesHandler/icons/film_add.png"), tr("O&pen"), this );
	openVideoAction->setShortcut( tr("Ctrl+O") );
	openVideoAction->setStatusTip( tr("Open video file") );
	connect( openVideoAction, SIGNAL( triggered() ), this, SLOT( openVideoFile() ) );

	deleteVideoAction = new QAction( QIcon(":/FramesHandler/icons/film_delete.png"), tr("C&lose"), this );
	deleteVideoAction->setShortcut( tr("Ctrl+D") );
	deleteVideoAction->setStatusTip( tr("Close video file") );
	deleteVideoAction->setEnabled(false);
	connect( deleteVideoAction, SIGNAL( triggered() ), this, SLOT( deleteVideoFile() ) );

	createROIAction = new QAction( QIcon(":/FramesHandler/icons/cornerPoints_notSet.png"), tr("Set ROI"), this );
	createROIAction->setShortcut( tr("Ctrl+R") );
	createROIAction->setStatusTip( tr("Create ROI") );
	createROIAction->setCheckable(true);
	createROIAction->setEnabled(false);
	connect( createROIAction, SIGNAL( triggered(bool) ), this, SLOT( createROISlot(bool) ) );

	saveROIAction = new QAction( QIcon(":/FramesHandler/icons/cornerPoints_save.png"), tr("Save ROI"), this );
	saveROIAction->setStatusTip( tr("Save ROI") );
	saveROIAction->setEnabled(false);
	connect( saveROIAction, SIGNAL( triggered() ), this, SLOT( saveROISlot() ) );

	calcFrameProperiesAction = new QAction( QIcon(":/FramesHandler/icons/color_adjustment.png"), tr("Calc MBB and MDR"), this );
	calcFrameProperiesAction->setShortcut( tr("Ctrl+P") );
	calcFrameProperiesAction->setStatusTip( tr("Calculate frame properties: MBB and DR") );
	calcFrameProperiesAction->setEnabled(false);
	connect( calcFrameProperiesAction, SIGNAL( triggered() ), this, SLOT( calcFrameProperies() ) );

	oneFrameModeAction = new QAction("One frame handle mode", this);
	oneFrameModeAction->setCheckable(true);
	oneFrameModeAction->setChecked(true);
	connect( oneFrameModeAction, SIGNAL( triggered() ), this, SLOT( changeModeSlot() ) );

	videoModeAction = new QAction("Video handle mode", this);
	videoModeAction->setCheckable(true);
	connect( videoModeAction, SIGNAL( triggered() ), this, SLOT( changeModeSlot() ) );
	videoModeAction->setEnabled(true);

	cameraModeAction = new QAction("Camera mode", this);
	cameraModeAction->setCheckable(true);
	connect( cameraModeAction, SIGNAL( triggered() ), this, SLOT( changeModeSlot() ) );

	modeActionGroup = new QActionGroup(this);
	modeActionGroup->setExclusive(true);
	modeActionGroup->addAction(oneFrameModeAction);
	modeActionGroup->addAction(videoModeAction);
	modeActionGroup->addAction(cameraModeAction);

	OF_Type1_AlgAction = new QAction( tr("OF_Algorithm 1"), this );
	OF_Type1_AlgAction->setCheckable(true);
	connect(OF_Type1_AlgAction, SIGNAL( triggered() ), this, SLOT( changeAlgorithmSlot() ) );
	
	OF_Type2_AlgAction = new QAction( tr("OF_Algorithm 2"), this );
	OF_Type2_AlgAction->setCheckable(true);
	OF_Type2_AlgAction->setChecked(true);
	connect(OF_Type2_AlgAction, SIGNAL( triggered() ), this, SLOT( changeAlgorithmSlot() ) );

	OF_Type3_AlgAction = new QAction( tr("OF_Algorithm 3"), this );
	OF_Type3_AlgAction->setCheckable(true);
	OF_Type3_AlgAction->setChecked(false);
	connect(OF_Type3_AlgAction, SIGNAL( triggered() ), this, SLOT( changeAlgorithmSlot() ) );

	AF_Type1_AlgAction = new QAction( tr("AF_Algorithm 1"), this );
	AF_Type1_AlgAction->setCheckable(true);
	connect(AF_Type1_AlgAction, SIGNAL( triggered() ), this, SLOT( changeAlgorithmSlot() ) );
	AF_Type1_AlgAction->setEnabled(false);

	AF_Type2_AlgAction = new QAction( tr("AF_Algorithm 2"), this );
	AF_Type2_AlgAction->setCheckable(true);
	connect(AF_Type2_AlgAction, SIGNAL( triggered() ), this, SLOT( changeAlgorithmSlot() ) );
	AF_Type2_AlgAction->setEnabled(false);

	algorithmActionGroup = new QActionGroup(this);
	algorithmActionGroup->setExclusive(true);
	algorithmActionGroup->addAction(OF_Type1_AlgAction);
	algorithmActionGroup->addAction(OF_Type2_AlgAction);
	algorithmActionGroup->addAction(OF_Type3_AlgAction);
	algorithmActionGroup->addAction(AF_Type1_AlgAction);
	algorithmActionGroup->addAction(AF_Type2_AlgAction);
}

void FrameHandlerMWindow::createMenus()
{
	fileMenu = menuBar()->addMenu( tr("&File") );
	fileMenu->addAction(openVideoAction);
	fileMenu->addAction(deleteVideoAction);

	viewMenu = menuBar()->addMenu( tr("&View") );
	viewMenu->addAction(createROIAction);
	viewMenu->addAction(saveROIAction);
	viewMenu->addSeparator();
	viewMenu->addAction(calcFrameProperiesAction);

	modeMenu = menuBar()->addMenu( tr("&Mode") );
	modeMenu->addAction(oneFrameModeAction);
	modeMenu->addAction(videoModeAction);
	modeMenu->addAction(cameraModeAction);

	algorythmMenu = menuBar()->addMenu( tr("&Algorithm") );
	algorythmMenu->addAction(OF_Type1_AlgAction);
	algorythmMenu->addAction(OF_Type2_AlgAction);
	algorythmMenu->addAction(OF_Type3_AlgAction);
	algorythmMenu->addSeparator();
	algorythmMenu->addAction(AF_Type1_AlgAction);
	algorythmMenu->addAction(AF_Type2_AlgAction);
}

void FrameHandlerMWindow::createVideoPathLE()
{
	videoPathLineEdit = new QLineEdit(this);
	videoPathLineEdit->setReadOnly(true);

	videoPathLineEdit->setMinimumWidth(250);
	videoPathLineEdit->setMaximumWidth(400);
	videoPathLineEdit->setToolTip( tr("Path of the current video file") );
}

void FrameHandlerMWindow::createToolBarServWgt()
{
	roiFramesEnableChBox = new QCheckBox( tr("ROI show") );
	roiFramesEnableChBox->setChecked(true);

	QHBoxLayout *roiServLayout = new QHBoxLayout(this);
	roiServLayout->addWidget(roiFramesEnableChBox);

	roiServWgt = new QWidget();
	roiServWgt->setLayout(roiServLayout);
}

void FrameHandlerMWindow::createFilters()
{
	sensitivityLabel = new QLabel( tr("Sensitivity: ") );
	
	sensitivitySpBox = new QSpinBox();
	sensitivitySpBox->setMinimum(0);
	sensitivitySpBox->setValue(4);
	sensitivitySpBox->setEnabled(false);
	connect(sensitivitySpBox, SIGNAL( valueChanged(int) ), frameHandleAlgorithm.data(), SLOT( setSensitivity(int) ) );

	sizeFactorLabel = new QLabel( tr("Size factor: ") );
	
	sizeFactorSpBox = new QSpinBox();
	sizeFactorSpBox->setMinimum(1);
	sizeFactorSpBox->setMaximum(50);
	sizeFactorSpBox->setValue(5);
	sizeFactorSpBox->setEnabled(false);
	connect(sizeFactorSpBox, SIGNAL( valueChanged(int) ), frameHandleAlgorithm.data(), SLOT( setSizeFactor(int) ) );
	
	thresholdLabel = new QLabel( tr("Threshold: ") );
	thresholdLabel->setIndent(5);

	thresholdSpBox = new QSpinBox();
	thresholdSpBox->setMinimum(1);
	thresholdSpBox->setMaximum(500);
	thresholdSpBox->setValue(10);
	thresholdSpBox->setEnabled(false);
	connect(thresholdSpBox, SIGNAL( valueChanged(int) ), frameHandleAlgorithm.data(), SLOT( setThreshold(int) ) );

	minSumBrightnessLabel = new QLabel("MBT: ");
	minSumBrightnessLabel->setIndent(5);

	minSumBrightnessSpBox = new QSpinBox();
	minSumBrightnessSpBox->setMinimum(1);
	minSumBrightnessSpBox->setMaximum(20000);
	minSumBrightnessSpBox->setValue(500);
	minSumBrightnessSpBox->setEnabled(false);
	connect( minSumBrightnessSpBox, SIGNAL( valueChanged(int) ),
			 frameHandleAlgorithm.data(), SLOT( setMBT(int) ) );

	minShootBrFactorLabel = new QLabel("MHBF: ");
	minShootBrFactorLabel->setIndent(5);
	
	minShootBrFactorSpBox = new QDoubleSpinBox();
	minShootBrFactorSpBox->setMinimum(0.0);
	minShootBrFactorSpBox->setMaximum(100.0);
	minShootBrFactorSpBox->setValue(2.0);
	minShootBrFactorSpBox->setSingleStep(0.1);
	minShootBrFactorSpBox->setEnabled(false);
	connect( minShootBrFactorSpBox, SIGNAL( valueChanged(double) ),
		     frameHandleAlgorithm.data(), SLOT( setMHBF(double) ) ); 
	//TODO add the SIGNAL/SLOT connection

	QHBoxLayout *filtersLayout = new QHBoxLayout();
	filtersLayout->addWidget(sizeFactorLabel);
	filtersLayout->addWidget(sizeFactorSpBox);
	filtersLayout->addWidget(thresholdLabel);
	filtersLayout->addWidget(thresholdSpBox);
	filtersLayout->addWidget(minSumBrightnessLabel);
	filtersLayout->addWidget(minSumBrightnessSpBox);
	filtersLayout->addWidget(minShootBrFactorLabel);
	filtersLayout->addWidget(minShootBrFactorSpBox);
		
	filtersGrBox = new QGroupBox( tr("Filters") );
	filtersGrBox->setAlignment(Qt::AlignCenter);
	filtersGrBox->setLayout(filtersLayout);
}

QToolBar* FrameHandlerMWindow::createToolBar()
{
	mainToolBar = new QToolBar( tr("Main ToolBar") ); 
	mainToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	mainToolBar->addAction(openVideoAction);
	mainToolBar->addAction(deleteVideoAction);
	mainToolBar->addWidget(videoPathLineEdit);
	mainToolBar->addSeparator();
	mainToolBar->addAction(createROIAction);
	mainToolBar->addAction(saveROIAction);
	mainToolBar->addWidget(roiServWgt);
	mainToolBar->addSeparator();
	mainToolBar->addAction(calcFrameProperiesAction);
	mainToolBar->addSeparator();
	//mainToolBar->addSeparator();

	//QWidget *emptyWgt = new QWidget(this);
	//emptyWgt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//mainToolBar->addWidget(emptyWgt);
	mainToolBar->addWidget(filtersGrBox);
	mainToolBar->addSeparator();

	return mainToolBar;
}

void FrameHandlerMWindow::createPixmapLabel()
{
	framesPixmapLabel = new PixmapRoiLabel(this);
	framesPixmapLabel->setFrame(true);
	framesPixmapLabel->enableOverScaling(false);
		framesPixmapLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	centerLayout = new QHBoxLayout(this);
	centerLayout->addWidget(framesPixmapLabel);

	connect( framesPixmapLabel, SIGNAL( setRoi(QPoint &, QPoint &) ),
			 frameHandleAlgorithm.data(), SLOT( setRoi(QPoint &, QPoint &) ) );
	connect( framesPixmapLabel, SIGNAL( clearRoiRect() ),
			 this, SLOT( resetRoiRect() ) );
	connect( roiFramesEnableChBox, SIGNAL( toggled(bool) ), 
			 framesPixmapLabel, SLOT( drawROI(bool) ) );
	connect( createROIAction, SIGNAL( triggered(bool) ), 
			 framesPixmapLabel, SLOT( drawCurrentROI(bool) ) );
}

void FrameHandlerMWindow::createResultWgt()
{
	resWgt = new FramesHandlerResultWgt(this);
	centerLayout->addWidget(resWgt);
}

void FrameHandlerMWindow::createSlider()
{
	moveSlider = new QSlider(Qt::Horizontal, this);
	moveSlider->setTickPosition(QSlider::NoTicks);
	moveSlider->setEnabled(false);
	sliderLayout = new QHBoxLayout(this);
	sliderLayout->addWidget(moveSlider);
	connect( moveSlider, SIGNAL( sliderMoved(int) ), this, SLOT( moveSliderSlot(int) ) );
	connect( moveSlider, SIGNAL( valueChanged(int) ), this, SLOT( moveSliderSlot(int) ) );
}

void FrameHandlerMWindow::createButtons()
{
	playButton = new QPushButton(this);
	playButton->setIcon( QIcon(":/FramesHandler/icons/control_play.png") );
	playButton->setIconSize( QSize(35, 35) );
	playButton->setToolTip( tr("Play") );
	playButton->setMinimumSize(40, 40);
	playButton->setFlat(true);
	playButton->setEnabled(false);
	connect( playButton, SIGNAL( clicked() ), this, SLOT( playVideo() ) );

	pauseButton = new QPushButton(this);
	pauseButton->setIcon( QIcon(":/FramesHandler/icons/control_pause.png") );
	pauseButton->setIconSize( QSize(35, 35) );
	pauseButton->setToolTip( tr("Pause") );
	pauseButton->setMinimumSize(40, 40);
	pauseButton->setFlat(true);
	pauseButton->setEnabled(false);
	connect( pauseButton, SIGNAL( clicked() ), this, SLOT( pauseVideo() ) );

	stopButton = new QPushButton(this);
	stopButton->setIcon( QIcon(":/FramesHandler/icons/control_stop.png") );
	stopButton->setIconSize( QSize(35, 35) );
	stopButton->setToolTip( tr("Stop") );
	stopButton->setMinimumSize(40, 40);
	stopButton->setFlat(true);
	stopButton->setEnabled(false);
	connect( stopButton, SIGNAL( clicked() ), this, SLOT( stopVideo() ) );

	prevFrameButton = new QPushButton(this);
	prevFrameButton->setIcon( QIcon(":/FramesHandler/icons/previous_frame.png") );
	prevFrameButton->setIconSize( QSize(35, 35) );
	prevFrameButton->setToolTip( tr("Previous frame") );
	prevFrameButton->setMinimumSize(40, 40);
	prevFrameButton->setFlat(true);
	prevFrameButton->setEnabled(false);
	connect( prevFrameButton, SIGNAL( clicked() ), this, SLOT( prevFrameSlot() ) );

	nextFrameButton = new QPushButton(this);
	nextFrameButton->setIcon( QIcon(":/FramesHandler/icons/next_frame.png") );
	nextFrameButton->setIconSize( QSize(35, 35) );
	nextFrameButton->setToolTip( tr("Next frame") );
	nextFrameButton->setMinimumSize(40, 40);
	nextFrameButton->setFlat(true);
	nextFrameButton->setEnabled(false);
	connect( nextFrameButton, SIGNAL( clicked() ), this, SLOT(nextFrameSlot() ) );

	clearResultsButton = new QPushButton(tr("Clear results"), this);
	clearResultsButton->setMinimumSize(80, 50);
	clearResultsButton->setEnabled(false);
	connect( clearResultsButton, SIGNAL( clicked() ), resWgt, SLOT( clearResults() ) );
	
	setOriginalFrameButton = new QPushButton(tr("Set first frame"), this);
	setOriginalFrameButton->setMinimumSize(80, 50);
	setOriginalFrameButton->setEnabled(false);
	setOriginalFrameButton->hide();
	connect( setOriginalFrameButton, SIGNAL( clicked() ), this, SLOT( setFirstFrame() ) );

	startFindLSPushButton = new QPushButton(tr("Find LS"), this);
	startFindLSPushButton->setToolTip( tr("Find laser spots in the current frame") );
	startFindLSPushButton->setMinimumSize(80, 50);
	startFindLSPushButton->setEnabled(false);
	connect( startFindLSPushButton, SIGNAL( clicked() ), this, SLOT( startFindLS() ) );
}

void FrameHandlerMWindow::createFramesCounterWgts()
{
	allFramesLabel = new QLabel( tr("Count frames:  ") );
	allFramesLabel->setMaximumHeight(35);
	allFramesLineEdit = new QLineEdit(this);
	allFramesLineEdit->setMaximumWidth(60);
	allFramesLineEdit->setReadOnly(true);

	QHBoxLayout *allFrameLayout = new QHBoxLayout();
	allFrameLayout->addWidget(allFramesLabel);
	allFrameLayout->addWidget(allFramesLineEdit);

	currentFrameLabel = new QLabel(tr("Current frame: "), this);
	currentFrameSpBox = new QSpinBox(this);
	currentFrameSpBox->setMinimum(0);
	currentFrameSpBox->setMaximumWidth(60);
	currentFrameSpBox->setEnabled(false);
	connect( currentFrameSpBox, SIGNAL( valueChanged(int) ), moveSlider, SLOT( setValue(int) ) );

	QHBoxLayout *curFrameLayout = new QHBoxLayout();
	curFrameLayout->addWidget(currentFrameLabel);
	curFrameLayout->addWidget(currentFrameSpBox);

	cntcenterLayout = new QVBoxLayout(this);
	cntcenterLayout->addLayout(allFrameLayout);
	cntcenterLayout->addLayout(curFrameLayout);
}

void FrameHandlerMWindow::createBrightnWgts()
{
	middleBackgrBrLabel = new QLabel( tr("MBB:  "), this );
	middleBackgrBrLabel->setToolTip( tr("Middle background brightness") );
	middleBackgrBrLineEdit = new QLineEdit(this);
	middleBackgrBrLineEdit->setReadOnly(true);
	middleBackgrBrLineEdit->setMaximumWidth(60);
	middleBackgrBrLineEdit->setToolTip( tr("Middle background brightness") );

	middleDynRangeLabel = new QLabel( tr("MDR: "), this );
	middleDynRangeLabel->setToolTip( tr("Middle dynamic range") );
	middleDynRangeLineEdit = new QLineEdit(this);
	middleDynRangeLineEdit->setReadOnly(true);
	middleDynRangeLineEdit->setMaximumWidth(60);
	middleDynRangeLineEdit->setToolTip( tr("Middle dynamic range") );

	frameBrightnLayout = new QGridLayout(this);
	frameBrightnLayout->addWidget(middleBackgrBrLabel, 0, 0);
	frameBrightnLayout->addWidget(middleBackgrBrLineEdit, 0, 1);
	frameBrightnLayout->addWidget(middleDynRangeLabel, 1, 0);
	frameBrightnLayout->addWidget(middleDynRangeLineEdit, 1, 1);
}

void FrameHandlerMWindow::createViewLayout()
{
	viewLayout = new QHBoxLayout(this);
	viewLayout->addLayout(centerLayout);
}

void FrameHandlerMWindow::createControlLayout()
{
	controlLayout = new QHBoxLayout(this);
	controlLayout->addWidget(playButton);
	controlLayout->addWidget(pauseButton);
	controlLayout->addWidget(stopButton);
	controlLayout->addStretch(1);
	controlLayout->addWidget(prevFrameButton);
	controlLayout->addWidget(nextFrameButton);
	controlLayout->addStretch(1);
	controlLayout->addLayout(cntcenterLayout);
	controlLayout->addStretch(1);
	controlLayout->addLayout(frameBrightnLayout);
	controlLayout->addStretch(15);
	controlLayout->addWidget(clearResultsButton);
	controlLayout->addWidget(setOriginalFrameButton);
	controlLayout->addWidget(startFindLSPushButton);
}

void FrameHandlerMWindow::createMainWgt()
{
	mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(viewLayout);
	mainLayout->addLayout(sliderLayout);
	mainLayout->addLayout(controlLayout);
	mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);
}

/*slot*/void FrameHandlerMWindow::setImage(const QImage &newFrame)
{
	QPixmap currentFrame = QPixmap::fromImage(newFrame);
	framesPixmapLabel->setScaledPixmap(currentFrame);
	framesPixmapLabel->repaint();
	
	//TODO
	//отправка кадра на обработку если выбран режим обработки потока
	if( (m_videoMode || m_cameraMode) && (m_startHandle) )
	{
		frameHandleAlgorithm->setFrame( framesPixmapLabel->getFrame() );
		frameHandleAlgorithm->setNumOfFrame( currentFrameSpBox->value() );
		frameHandleAlgorithm->frameHandle();
	}
	
	update();
}

/*slot*/void FrameHandlerMWindow::setAllFrames(int framesCount)
{
	currentFrameSpBox->setMaximum(framesCount);
	allFramesLineEdit->setText( QString("%1").arg(framesCount) );
	initSlider();
}

/*slot*/void FrameHandlerMWindow::setFrameProperties(int midBr, int midDynRange)
{
	middleBackgrBrLineEdit->setText( QString("%1").arg(midBr) );
	middleDynRangeLineEdit->setText( QString("%1").arg(midDynRange) );
}

void FrameHandlerMWindow::clearFrameProperties()
{
	middleBackgrBrLineEdit->clear();
	middleDynRangeLineEdit->clear();
}

/*slot*/void FrameHandlerMWindow::openVideoFile()
{
	QString videoPath = QFileDialog::getOpenFileName(0, "Open video", "", "*.avi");
	if( !videoPath.isEmpty() )
	{
		videoPathLineEdit->setText(videoPath);
		m_framesHandler->openFile(videoPath);
				
		createROIAction->setIcon( QIcon(":/FramesHandler/icons/cornerPoints_notSet.png") );
		calcFrameProperiesAction->setEnabled(true);
		enableActions(true, true, true, false, true);
		filtersEnable(true);
		enableControls(true, true, false, false, false, true, true);

		if(!m_videoMode) enableMainButtons(true, true, true);
		else if(m_videoMode || m_cameraMode)
		{
			enableMainButtons(true, false, false);
			enableControls(true, true, false, false, false, false, false);
		}

		clearFrameProperties();
		clearRoiRect();		
	}
}

/*slot*/void FrameHandlerMWindow::deleteVideoFile()
{
	m_framesHandler->resetFile();
	videoPathLineEdit->clear();
	frameHandleAlgorithm->reset();

	moveSlider->setTickPosition(QSlider::NoTicks);
	moveSlider->setValue(0);

	createROIAction->setIcon( QIcon(":/FramesHandler/icons/cornerPoints_notSet.png") );
	enableActions(true, false, false, false, false);
	filtersEnable(false);
	enableControls(false, false, false, false, false, false, false);
	enableMainButtons(false, false, false);

	allFramesLineEdit->clear();
	clearFrameProperties();
	resWgt->clearResults();

	framesPixmapLabel->removePixmap();
	framesPixmapLabel->update();
}

/*slot*/void FrameHandlerMWindow::changeModeSlot()
{
	framesPixmapLabel->removePixmap();
	QAction *whatIsAction = qobject_cast<QAction*>( sender() );

	if(oneFrameModeAction == whatIsAction) 
	{
		initMode(false, false);
		startFindLSPushButton->setCheckable(false);
		if( !videoPathLineEdit->text().isEmpty() )
		{
			enableMainButtons(true, true, true);
			m_framesHandler->openFile( videoPathLineEdit->text() );
		}
		enableVideoAlgorithms(false);
		setOriginalFrameButton->hide();
		startFindLSPushButton->show();

		OF_Type2_AlgAction->setChecked(true);

		if( !openVideoAction->isEnabled() )
		{
			playButton->setEnabled(false);
			enableActions(true, false, false, false, false);
			filtersEnable(false);
			enableMainButtons(true, false, false);
			allFramesLineEdit->setEnabled(true);
		}
		if( !videoPathLineEdit->text().isEmpty() )
			enableControls(true, true, false, false, false, true, true);
		
		currentFrameSpBox->setEnabled(true);
	}
	else if(videoModeAction == whatIsAction) 
	{
		initMode(true, false);
		enableControls(false, true, false, false, false, false, false); //??
		enableMainButtons(true, false, false);
		if( !videoPathLineEdit->text().isEmpty() )
		{
			playButton->setEnabled(true);
			m_framesHandler->openFile( videoPathLineEdit->text() );
		}
		else
		{
			openVideoAction->setEnabled(true);
			playButton->setEnabled(false);
		}
		enableVideoAlgorithms(true);
		AF_Type2_AlgAction->setChecked(true);
		setOriginalFrameButton->hide();
		startFindLSPushButton->hide();
	}
	else if(cameraModeAction == whatIsAction)
	{
		initMode(false, true);
		
		startFindLSPushButton->setCheckable(true); 
		startFindLSPushButton->show();
		setOriginalFrameButton->hide();

		enableVideoAlgorithms(true);
		AF_Type2_AlgAction->setChecked(true);

		videoPathLineEdit->clear();
		enableActions(false, false, true, false, true);
		filtersEnable(true);
		enableControls(false, false, false, false, false, false, false);
		enableMainButtons(true, false, false); //?? TODO  - findLS - проверить
		currentFrameSpBox->setValue(0);
		allFramesLineEdit->clear();
		allFramesLineEdit->setEnabled(false);

		m_framesHandler->play();
	}
	changeAlgorithmSlot(1);
}

/*slot*/void FrameHandlerMWindow::changeAlgorithmSlot(int mode)
{
	frameHandleAlgorithm.clear();
	delete filtersGrBox->layout();

	QAction *whatIsAction = nullptr;
	if(!mode) whatIsAction = qobject_cast<QAction*>( sender() );
	else whatIsAction = algorithmActionGroup->checkedAction();

	if(OF_Type1_AlgAction == whatIsAction || AF_Type1_AlgAction == whatIsAction)
	{
		if(OF_Type1_AlgAction == whatIsAction)
			frameHandleAlgorithm = QSharedPointer<FLSAlgorithm>( algCreator.create(FAC::ONE_FRAME_TYPE1) );
		else frameHandleAlgorithm = QSharedPointer<FLSAlgorithm>( algCreator.create(FAC::ALL_FRAMES_TYPE1) );
		createFilter1();
		update();
	}
	else if( (OF_Type2_AlgAction == whatIsAction) ||
			 (OF_Type3_AlgAction == whatIsAction) ||
			 (AF_Type2_AlgAction == whatIsAction) )
	{
		frameHandleAlgorithm = QSharedPointer<FLSAlgorithm>( algCreator.create(FAC::ONE_FRAME_TYPE2) );
		createFilter2();
		update();
		
		if(OF_Type3_AlgAction == whatIsAction)
		{
			frameHandleAlgorithm = QSharedPointer<FLSAlgorithm>( algCreator.create(FAC::ONE_FRAME_TYPE3) );
			setOriginalFrameButton->show();
		}
		else if(AF_Type2_AlgAction == whatIsAction)
			frameHandleAlgorithm = QSharedPointer<FLSAlgorithm>( algCreator.create(FAC::ALL_FRAMES_TYPE2) );
		else setOriginalFrameButton->hide();
	}
	setFilters();
	reconnectAllSignals();
	framesPixmapLabel->saveROI(true, false);
}

void FrameHandlerMWindow::setFilters()
{
	frameHandleAlgorithm->setSizeFactor( sizeFactorSpBox->value() );
	frameHandleAlgorithm->setThreshold( thresholdSpBox->value() );
	frameHandleAlgorithm->setSensitivity( sensitivitySpBox->value() );
	frameHandleAlgorithm->setMBT( minSumBrightnessSpBox->value() );
	frameHandleAlgorithm->setMHBF( minShootBrFactorSpBox->value() );
}

void FrameHandlerMWindow::enableVideoAlgorithms(bool flag)
{
	if(flag)
	{
		AF_Type1_AlgAction->setEnabled(true);
		AF_Type2_AlgAction->setEnabled(true);
		OF_Type1_AlgAction->setEnabled(false);
		OF_Type2_AlgAction->setEnabled(false);
		OF_Type3_AlgAction->setEnabled(false);
	}
	else
	{
		AF_Type1_AlgAction->setEnabled(false);
		AF_Type2_AlgAction->setEnabled(false);
		OF_Type1_AlgAction->setEnabled(true);
		OF_Type2_AlgAction->setEnabled(true);
		OF_Type3_AlgAction->setEnabled(true);
	}
}

void FrameHandlerMWindow::reconnectAllSignals()
{
	disconnect(frameHandleAlgorithm.data(), SIGNAL( setFrameProperties(int, int), 0, 0) );
	disconnect(frameHandleAlgorithm.data(), SIGNAL( valueChanged(int), 0, 0) );
	disconnect(sizeFactorSpBox, 0,  frameHandleAlgorithm.data(), 0);
	disconnect(thresholdSpBox, 0, frameHandleAlgorithm.data(), 0);
	disconnect(framesPixmapLabel, SIGNAL( setRoi(const QList<QRect> &) ), 0, 0);
	disconnect(minSumBrightnessSpBox, 0, frameHandleAlgorithm.data(), 0);
	disconnect(minShootBrFactorSpBox, 0, frameHandleAlgorithm.data(), 0); 
	disconnect(frameHandleAlgorithm.data(), SIGNAL( sendResults(const HandleResult &) ), 0, 0);

	connect( frameHandleAlgorithm.data(), SIGNAL( setFrameProperties(int, int) ),
			 this, SLOT( setFrameProperties(int, int) ) );
	connect( sensitivitySpBox, SIGNAL( valueChanged(int) ), 
			 frameHandleAlgorithm.data(), SLOT( setSensitivity(int) ) );
	connect( sizeFactorSpBox, SIGNAL( valueChanged(int) ), 
			 frameHandleAlgorithm.data(), SLOT( setSizeFactor(int) ) );
	connect( thresholdSpBox, SIGNAL( valueChanged(int) ),
			 frameHandleAlgorithm.data(), SLOT( setThreshold(int) ) );
	connect( minShootBrFactorSpBox, SIGNAL( valueChanged(double) ),
			 frameHandleAlgorithm.data(), SLOT( setMHBF(double) ) ); 
	connect( framesPixmapLabel, SIGNAL( setRoi(QPoint &, QPoint &) ),
			 frameHandleAlgorithm.data(), SLOT( setRoi(QPoint &, QPoint &) ) );
	connect( minSumBrightnessSpBox, SIGNAL( valueChanged(int) ),
			 frameHandleAlgorithm.data(), SLOT( setMBT(int) ) );
	connect( frameHandleAlgorithm.data(), SIGNAL( sendResults(const HandleResult &) ),
			 this, SLOT( writeResultToLog(const HandleResult &) ) );
}

void FrameHandlerMWindow::filtersEnable(bool flag)
{
	sensitivitySpBox->setEnabled(flag);
	sizeFactorSpBox->setEnabled(flag);
	thresholdSpBox->setEnabled(flag);
	minSumBrightnessSpBox->setEnabled(flag);
	minShootBrFactorSpBox->setEnabled(flag);
}

void FrameHandlerMWindow::enableActions(bool openFlag, bool closeFlag,
										bool roiFlag, bool saveRoiFlag,
										bool clacFramePropFlag)
{
	openVideoAction->setEnabled(openFlag);
	deleteVideoAction->setEnabled(closeFlag);
	createROIAction->setEnabled(roiFlag);
	saveROIAction->setEnabled(saveRoiFlag);
	calcFrameProperiesAction->setEnabled(clacFramePropFlag);
}

void FrameHandlerMWindow::enableControls(bool sliderFlag, bool playFlag, bool pauseFlag,
								         bool stopFlag, bool prFrameFlag, bool nextFrameFlag,
										 bool currframeSpBoxFlag)
{
	moveSlider->setEnabled(sliderFlag);
	playButton->setEnabled(sliderFlag);
	pauseButton->setEnabled(pauseFlag);
	stopButton->setEnabled(stopFlag);
	prevFrameButton->setEnabled(prFrameFlag);
	nextFrameButton->setEnabled(nextFrameFlag);
	currentFrameSpBox->setEnabled(currframeSpBoxFlag);
}

void FrameHandlerMWindow::enableMainButtons(bool clearResult, bool firstFrame, bool startFind)
{
	clearResultsButton->setEnabled(clearResult);
	setOriginalFrameButton->setEnabled(firstFrame);
	startFindLSPushButton->setEnabled(startFind);
}

void FrameHandlerMWindow::initMode(bool videoModeFlag, bool cameraModeFlag)
{
	m_videoMode = videoModeFlag;
	m_cameraMode = cameraModeFlag;
	m_framesHandler->setCameraMode(m_cameraMode);
}

void FrameHandlerMWindow::createFilter1()
{
	sizeFactorLabel->hide();
	sizeFactorSpBox->hide();

	QHBoxLayout *filtersLayout = new QHBoxLayout(this);
	filtersLayout->addWidget(sensitivityLabel);
	filtersLayout->addWidget(sensitivitySpBox);
	filtersLayout->addWidget(thresholdLabel);
	filtersLayout->addWidget(thresholdSpBox);
	filtersLayout->addWidget(minSumBrightnessLabel);
	filtersLayout->addWidget(minSumBrightnessSpBox);
	filtersLayout->addWidget(minShootBrFactorLabel);
	filtersLayout->addWidget(minShootBrFactorSpBox);

	setOriginalFrameButton->hide();
	filtersGrBox->setLayout(filtersLayout);
	sensitivityLabel->show();
	sensitivitySpBox->show();
}

void FrameHandlerMWindow::createFilter2()
{
	sensitivityLabel->hide();
	sensitivitySpBox->hide();

	QHBoxLayout *filtersLayout = new QHBoxLayout(this);
	filtersLayout->addWidget(sizeFactorLabel);
	filtersLayout->addWidget(sizeFactorSpBox);
	filtersLayout->addWidget(thresholdLabel);
	filtersLayout->addWidget(thresholdSpBox);
	filtersLayout->addWidget(minSumBrightnessLabel);
	filtersLayout->addWidget(minSumBrightnessSpBox);
	filtersLayout->addWidget(minShootBrFactorLabel);
	filtersLayout->addWidget(minShootBrFactorSpBox);

	sizeFactorLabel->show();
	sizeFactorSpBox->show();

	filtersGrBox->setLayout(filtersLayout);
}

/*slot*/void FrameHandlerMWindow::playVideo()
{
	enableActions(false, false, false, false, false);
	filtersEnable(false);
	enableControls(false, false, true, true, false, false, false);
	enableMainButtons(true, false, false);

	m_startVideoState = true;
	if(m_videoMode)	m_startHandle = true;//need test
	if(m_videoMode && !wasSaved)
	{
		frameHandleAlgorithm->setResultFileName( QFileDialog::getSaveFileName(0, "Save result of frame handle", "", "*.txt") ); //??
		frameHandleAlgorithm->setVideoFilePath( videoPathLineEdit->text() );
		wasSaved = true;
	}
	m_framesHandler->play();
}

/*slot*/void FrameHandlerMWindow::pauseVideo()
{	
	enableActions(true, true, true, createROIAction->isChecked(), true);
	filtersEnable(true);
	enableControls(true, true, false, true, true, true, true);
	enableMainButtons(true, true, true);
	if(m_videoMode || m_cameraMode)
		enableControls(true, true, false, true, false, false, false);

	m_startHandle = false; //TODO
	m_framesHandler->pause();
}

/*slot*/void FrameHandlerMWindow::stopVideo()
{
	m_framesHandler->stop();
	enableActions(true, true, true, createROIAction->isChecked(), true);
	filtersEnable(true);
	enableControls(true, true, false, false, false, true, true);
	enableMainButtons(true, true, true);
	if(m_videoMode || m_cameraMode)
	{
		enableControls(true, true, false, false, false, false, false);
		wasSaved = false;
	}
	frameHandleAlgorithm->stopHandle();
}

/*slot*/void FrameHandlerMWindow::stopPlaying()
{
	if( ( !m_framesHandler->isStopped() ) && ( !m_framesHandler->isPaused() ) )
	{
		m_framesHandler->stop();
		wasSaved = false;
	}

	if( m_framesHandler->isPaused() ) m_startVideoState = true;
	else if( m_framesHandler->isStopped() )
	{
		m_startVideoState = false;
		wasSaved = false;
	}

	if(m_cameraMode)
	{
		enableActions(false, false, false, false, false);
		filtersEnable(false);
		//enableMainButtons(true, false, false); //old

		//---new test---//
		enableMainButtons(true, false, true);
		startFindLSPushButton->setChecked(false);
		m_startHandle = false;
		//--------------//
	}
	else
	{
		enableActions(true, true, true, createROIAction->isChecked(), true);
		filtersEnable(true);
		enableMainButtons(true, true, true);
	}

	if( !cameraModeAction->isChecked() ) playButton->setEnabled(true);
	pauseButton->setEnabled(false);

	if( !m_framesHandler->isPaused() )
	{
		stopButton->setEnabled(false);
		moveSlider->setValue( moveSlider->minimum() );
		if(m_videoMode) 
		{
			enableControls(true, false, false, false, false, false, false);
			resWgt->writeEndOfHandle();
		}
	}

	frameHandleAlgorithm->stopHandle(); 
	m_startHandle = false; 
}

/*slot*/void FrameHandlerMWindow::moveSliderSlot(int val)
{
	if(!m_startVideoState)
	{
		if( moveSlider->maximum() == moveSlider->value() )
			nextFrameButton->setEnabled(false);
		else if( !nextFrameButton->isEnabled() ) nextFrameButton->setEnabled(true);

		if( moveSlider->minimum() == moveSlider->value() )
			prevFrameButton->setEnabled(false);
		else if( !prevFrameButton->isEnabled() ) prevFrameButton->setEnabled(true);
		m_framesHandler->setFrame(val);
	}

	if( (m_startVideoState) && ( m_framesHandler->isPaused() ) ) m_framesHandler->setFrame(val);
	currentFrameSpBox->setValue(val);
}

/*slot*/void FrameHandlerMWindow::prevFrameSlot()
{
	currentFrameSpBox->setValue(currentFrameSpBox->value() - 1);
	moveSlider->setValue( currentFrameSpBox->value() );
	if(  currentFrameSpBox->value() == currentFrameSpBox->minimum() )
		prevFrameButton->setEnabled(false);
	if( !nextFrameButton->isEnabled() ) nextFrameButton->setEnabled(true);

	m_framesHandler->setFrame( currentFrameSpBox->value() );
}

/*slot*/void FrameHandlerMWindow::nextFrameSlot()
{
	currentFrameSpBox->setValue(currentFrameSpBox->value() + 1);
	moveSlider->setValue( currentFrameSpBox->value() );
	if( currentFrameSpBox->value() == currentFrameSpBox->maximum() )
		nextFrameButton->setEnabled(false);
	if( !prevFrameButton->isEnabled() ) prevFrameButton->setEnabled(true);

	m_framesHandler->setFrame( currentFrameSpBox->value() );
}

void FrameHandlerMWindow::initSlider()
{
	moveSlider->setEnabled(true);
	moveSlider->setRange( currentFrameSpBox->minimum(), currentFrameSpBox->maximum() );
	moveSlider->setValue(0);
	moveSlider->setTickInterval(1);
	moveSlider->setTickPosition(QSlider::TicksAbove);
}

void FrameHandlerMWindow::createROISlot(bool check)
{
	if(check) 
	{
        createROIAction->setIcon( QIcon(":/FramesHandler/icons/cornerPoints_current.png") );
		framesPixmapLabel->setCursor(Qt::CrossCursor);
		saveROIAction->setEnabled(true);
	}
	else
	{
		if( framesPixmapLabel->roiSaved() ) 
			createROIAction->setIcon( QIcon(":/FramesHandler/icons/cornerPoints_set.png") );
		else createROIAction->setIcon( QIcon(":/FramesHandler/icons/cornerPoints_notSet.png") );
		framesPixmapLabel->setCursor(Qt::ArrowCursor);
		saveROIAction->setEnabled(false);
	}
}

void FrameHandlerMWindow::saveROISlot()
{
	framesPixmapLabel->saveROI(true, true);
	createROIAction->setChecked(false);
	createROISlot(false);
}

void FrameHandlerMWindow::clearRoiRect()
{
	framesPixmapLabel->setCursor(Qt::ArrowCursor);
	createROIAction->setChecked(false);
	framesPixmapLabel->clearRoi(); //??
}

void FrameHandlerMWindow::resetRoiRect()
{
	if( createROIAction->isChecked() )
		createROIAction->setIcon( QIcon(":/FramesHandler/icons/cornerPoints_current.png") );
}

void FrameHandlerMWindow::testSlot(const QPoint &p1, const QPoint &p2)
{
	//QContextMenuEvent *cme = new QContextMenuEvent(QContextMenuEvent::Mouse, p2);
	//QApplication::sendEvent(framesPixmapLabel, cme);
}

//test
void FrameHandlerMWindow::calcFrameProperies()
{
	frameHandleAlgorithm->setFrame( framesPixmapLabel->getFrame() );
	frameHandleAlgorithm->calcFrameProperties();
}

/*slot*/void FrameHandlerMWindow::setFirstFrame()
{
	frameHandleAlgorithm->setFrame( framesPixmapLabel->getFrame() );
	frameHandleAlgorithm->frameHandle();
}

void FrameHandlerMWindow::startFindLS()
{
	if( startFindLSPushButton->isCheckable() )
	{
		frameHandleAlgorithm->setResultFileName( QFileDialog::getSaveFileName(0, "Save result of frame handle", "", "*.txt") ); //??
		if( startFindLSPushButton->isChecked() )
		{
			//m_cameraMode = true;
			if( !framesPixmapLabel->roiSaved() ) return;
			m_startHandle = true;
		}
		else 
		{
			//m_cameraMode = false;
			m_startHandle = false;
		}
	}
	else
	{
 		frameHandleAlgorithm->setFrame( framesPixmapLabel->getFrame() );
		frameHandleAlgorithm->setResultFileName( QFileDialog::getSaveFileName(0, "Save result of frame handle", "", "*.txt") );
		frameHandleAlgorithm->setVideoFilePath( videoPathLineEdit->text() );
		frameHandleAlgorithm->setNumOfFrame( currentFrameSpBox->value() );
		frameHandleAlgorithm->frameHandle();
	}
}

void FrameHandlerMWindow::writeResultToLog(const HandleResult &result)
{
	resWgt->writeResult(result);
}

FrameHandlerMWindow::~FrameHandlerMWindow() { ; }