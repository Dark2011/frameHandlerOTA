#pragma once


//test
#include <QtCore/QThread>

class QString;
class QImage;
class QPoint;

class FramesHandlerKernelIFace : public QThread
{
	Q_OBJECT

public:
	FramesHandlerKernelIFace(QObject *parent = 0) : QThread(parent) { ; }
	~FramesHandlerKernelIFace() { ; }

signals:
	void setImage(const QImage &);
	void setAllFrames(int);
	void setCurrentMovePos(int);

public slots:
	virtual void play() = 0;
	virtual void stop() = 0;
	virtual bool isStopped() const = 0;
	virtual void pause() = 0;
	virtual bool isPaused() const = 0;
	virtual bool setFrame(int) = 0;
	//virtual void nextFrame() = 0;
	//virtual void prevFrame() = 0;
	virtual void setRoi(QPoint &, QPoint &) = 0; //перенести в FLSAlgorithm
	
	virtual void openFile(const QString &) = 0;
	virtual void resetFile() = 0;

	virtual void setCameraMode(bool) = 0;

	//virtual void changeFrameHandlerAlghorithm(int) = 0;
	//virtual void calcMiddleBackgrBrightness() = 0;
	//virtual void calcMiddleDynamicRange() = 0;
	//virtual void handleCurrentFrame() = 0;
};