#pragma once
//qt
#include <QtGui/QMainWindow>
#include <QtCore/QSharedPointer>
//own
#include "FLSAlgorithmsCreator.h"
#include "frameHandlerResWgt.h"

class QMenu;
class QAction;
class PixmapRoiLabel;
class FramesHandlerKernelIFace;
class QTableWidget;
class QHBoxLayout;
class QVBoxLayout;
class QWidget;
class QImage;
class QPushButton;
class QSpinBox;
class QLabel;
class QSlider;
class QToolBar;
class QLineEdit;
class QGroupBox;
class QGridLayout;
class QActionGroup;
class QCheckBox;

class FLSAlgorithm;
struct HandleResult;

class FrameHandlerMWindow : public QMainWindow
{
	Q_OBJECT

public:
	FrameHandlerMWindow(QWidget *parent = 0);
	~FrameHandlerMWindow();

public slots:
	void setImage(const QImage &);
	void setAllFrames(int);
	void setFrameProperties(int, int);
	void resetRoiRect();

	void testSlot(const QPoint &, const QPoint &);

private:
	void createActions();
	void createMenus();
	void createVideoPathLE();
	void createToolBarServWgt();
	void createFilters();
	QToolBar* createToolBar();
	void createPixmapLabel();
	void createResultWgt();
	void createSlider();
	void createButtons();
	void createFramesCounterWgts();
	void createBrightnWgts();
	void createViewLayout();
	void createControlLayout();
	void createMainWgt();

	void initSlider();
	void clearRoiRect();
	void clearFrameProperties();

	void setFilters();
	void reconnectAllSignals();

	void filtersEnable(bool);
	void enableVideoAlgorithms(bool);
	void enableActions(bool openFlag, bool closeFlag, bool roiFlag, bool saveRoiFlag, bool clacFramePropFlag);
	void enableControls(bool sliderFlag, bool playFlag, bool pauseFlag, bool stopFlag, bool prFrameFlag, bool nextFrameFlag, bool currframeSpBoxFlag);
	void enableMainButtons(bool clearResult, bool firstFrame, bool startFind);
	void initMode(bool, bool);
	void createFilter1();
	void createFilter2();

private slots:
	void openVideoFile();
	void deleteVideoFile();
	void changeModeSlot();
	void changeAlgorithmSlot(int = 0);
	void playVideo();
	void pauseVideo();
	void stopVideo();
	void stopPlaying();
	void createROISlot(bool);
	void saveROISlot();

	void moveSliderSlot(int);
	void prevFrameSlot();
	void nextFrameSlot();

	void calcFrameProperies();
	void setFirstFrame(); //TODO test
	void startFindLS();

	void writeResultToLog(const HandleResult &);
	
private:
	FramesHandlerKernelIFace * const m_framesHandler;
	bool m_startVideoState; 

	QAction *openVideoAction;
	QAction *deleteVideoAction;
	QAction *createROIAction;
	QAction *saveROIAction;
	QAction *calcFrameProperiesAction;

	QActionGroup *modeActionGroup;
	QAction *oneFrameModeAction;
	QAction *videoModeAction;
	QAction *cameraModeAction;

	bool m_videoMode;
	bool m_cameraMode;
	bool wasSaved;
	bool m_startHandle;

	QActionGroup *algorithmActionGroup;
	QAction *OF_Type1_AlgAction;
	QAction *OF_Type2_AlgAction;
	QAction *OF_Type3_AlgAction;
	QAction *AF_Type1_AlgAction;
	QAction *AF_Type2_AlgAction;

	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *modeMenu;
	QMenu *algorythmMenu;

	QLineEdit *videoPathLineEdit;

	QWidget *roiServWgt; 
	QCheckBox *roiFramesEnableChBox;
	
	QGroupBox *filtersGrBox;
	QLabel *sensitivityLabel;
	QSpinBox *sensitivitySpBox;
	QLabel *sizeFactorLabel;
	QSpinBox *sizeFactorSpBox;
	QLabel *thresholdLabel;
	QSpinBox *thresholdSpBox;
	QLabel *minSumBrightnessLabel;
	QSpinBox *minSumBrightnessSpBox;

	QToolBar *mainToolBar;
	
	QHBoxLayout *centerLayout;
	PixmapRoiLabel *framesPixmapLabel;
	FramesHandlerResultWgt *resWgt;

	QSlider *moveSlider;

	QPushButton *playButton;
	QPushButton *pauseButton;
	QPushButton *stopButton;
	QPushButton *prevFrameButton;
	QPushButton *nextFrameButton;

	QPushButton *clearResultsButton;
	QPushButton *setOriginalFrameButton; 
	QPushButton *startFindLSPushButton;
		
	QVBoxLayout *cntcenterLayout;
	QLabel *allFramesLabel;
	QLineEdit *allFramesLineEdit;
	QLabel *currentFrameLabel;
	QSpinBox *currentFrameSpBox;

	//QVBoxLayout *frameBrightnLayout;
	QGridLayout *frameBrightnLayout;
	QLabel *middleBackgrBrLabel;
	QLineEdit *middleBackgrBrLineEdit;
	QLabel *middleDynRangeLabel;
	QLineEdit *middleDynRangeLineEdit;

	QHBoxLayout *viewLayout;
	QHBoxLayout *sliderLayout;
	QHBoxLayout *controlLayout;

	QVBoxLayout *mainLayout;
	QWidget *mainWidget;

	FLSAlgorithmsCreator algCreator;
	QSharedPointer<FLSAlgorithm>frameHandleAlgorithm;
};

