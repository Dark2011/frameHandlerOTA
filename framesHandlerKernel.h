#pragma once
//own
#include "framesHandlerKernel_IFace.h"
#include "ProcessTimeCouter.h"
#include "FLSAlgorithmsCreator.h"
#include "FLSAlgorithm.h"
//qt
#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtCore/QRect>
//opencv
#include <opencv2/opencv.hpp>
#include <highgui.h>

class QPoint;

class VideoFileProperies
{
public:
	VideoFileProperies();
	
	void setFileName(const QString &);
	void setFramesCount(int);
	void setCurrentFrame(int);
	void setPause(bool);

	const QString & getFileName() const;
	int getFramesCount() const;
	int getCurrentFrame() const;
	bool wasPaused() const;

	void reset();
	
private:
	QString m_fileName;
	int m_framesCount;
	int m_currentFrame;

	bool was_paused;
};



class FramesHandlerKernel : public FramesHandlerKernelIFace
{
	Q_OBJECT

public:
	FramesHandlerKernel(QObject *parnet = 0);

protected:
	virtual void run();

private slots:
	//---------interface---------//
	virtual void play();
	virtual void stop();
	virtual bool isStopped() const;
	virtual void pause();
	virtual bool isPaused() const;
	virtual bool setFrame(int);
	virtual void setCameraMode(bool);

	//virtual void nextFrame();
	//virtual void prevFrame();
	virtual void setRoi(QPoint &, QPoint &);

	virtual void openFile(const QString &);
	virtual void resetFile();

private:
	void startPlayVideo();
	void startCamera();
	void readFirstFrame();
	void checkRoiCoords(QPoint &, QPoint &);
	void calcMiddleBackgrBrightness();
	void calcMiddleDynamicRange();
	void formationImage(const cv::Mat &);
	void internalFormation(const cv::Mat &); //test

private:
	VideoFileProperies m_videoFileProp;
	cv::VideoCapture m_capture;

	volatile bool m_stopped;
	volatile bool m_paused;
	volatile bool cameraMode;

	ProcessTimeCounter m_timeCounter;
	QRect m_roiRect;
};