//own
#include "framesHandlerKernel.h"
//qt
#include <QtGui/QImage>
#include <QtCore/QTime>
#include <QtCore/QPoint>

#include <QtCore/QDebug>
#include <QtGui/QRgb>

#define COLOR_IMAGE

VideoFileProperies::VideoFileProperies() :
m_fileName( QString() ), m_framesCount(0), m_currentFrame(0), was_paused(false) { ; }

void VideoFileProperies::setFileName(const QString &fileName)
{
	m_fileName = fileName;
}

void VideoFileProperies::setFramesCount(int framesCount)
{
	m_framesCount = framesCount;
}

void VideoFileProperies::setCurrentFrame(int currentFrame)
{
	m_currentFrame = currentFrame;
}

void VideoFileProperies::setPause(bool pauseFlag)
{
	was_paused = pauseFlag;
}

const QString & VideoFileProperies::getFileName() const
{
	return m_fileName;
}

int VideoFileProperies::getFramesCount() const
{
	return m_framesCount;
}

int VideoFileProperies::getCurrentFrame() const
{
	return m_currentFrame;
}

bool VideoFileProperies::wasPaused() const
{
	return was_paused;
}

void VideoFileProperies::reset()
{
	m_fileName = QString::null;
	m_framesCount = 0;
	m_currentFrame = 0;
	was_paused = false;;
}


FramesHandlerKernel::FramesHandlerKernel(QObject *parent) : 
FramesHandlerKernelIFace(parent), m_videoFileProp(), m_capture(0),
m_stopped(false), m_paused(false), m_roiRect( QRect() ), cameraMode(false) { ; }

/*virtual*/void FramesHandlerKernel::openFile(const QString &filename)
{
	if( m_capture.isOpened() ) m_capture.release();

	m_capture = cv::VideoCapture( filename.toStdString() );
	if( !m_capture.isOpened() ) return;

	m_videoFileProp.setFileName(filename);
	m_videoFileProp.setFramesCount( m_capture.get(CV_CAP_PROP_FRAME_COUNT) );

	emit setAllFrames( m_videoFileProp.getFramesCount() );

	readFirstFrame();
}

/*virtual*/void FramesHandlerKernel::resetFile()
{
	m_videoFileProp.reset();
	if( m_capture.isOpened() ) m_capture.release();
	m_stopped = true; //???
}

/*virtual*/void FramesHandlerKernel::play()
{
	start();
}

/*virtual*/void FramesHandlerKernel::pause()
{
	m_paused = true;
	//m_videoFileProp.setPause(true);
}

/*virtual*/void FramesHandlerKernel::run()
{
	m_stopped = false;
	m_paused = false;
	if(!cameraMode)	startPlayVideo();
	else startCamera();
}

/*virtual*/void FramesHandlerKernel::stop()
{
	m_stopped = true;
	if(m_paused) readFirstFrame();
}

/*virtual*/bool FramesHandlerKernel::setFrame(int numFrame)
{
	if( !m_capture.isOpened() ) return false;
	cv::Mat frame;
	m_capture.set(CV_CAP_PROP_POS_FRAMES, numFrame); 
	if( !m_capture.read(frame) ) return false;
	formationImage(frame);
	return true;
}

/*virtual*/void FramesHandlerKernel::setCameraMode(bool flag)
{
	cameraMode = flag;
}


/*virtual*/void FramesHandlerKernel::setRoi(QPoint &fp, QPoint &sp)
{
	checkRoiCoords(fp, sp);
	m_roiRect.setCoords( fp.x(), fp.y(), sp.x(), sp.y() );
	qDebug() << m_roiRect;
}

void FramesHandlerKernel::checkRoiCoords(QPoint &fp, QPoint &sp)
{
	if( ( fp.x() < sp.x() ) && ( fp.y() < sp.y() ) ) { ; }
	else if( ( fp.x() < sp.x() ) && ( fp.y() > sp.y() ) )
	{
		int tempY = fp.y();
		fp.setY( sp.y() );
		sp.setY(tempY);
	}
	else if( ( fp.x() > sp.x() ) && ( fp.y() < sp.y() ) )
	{
		int tempX = fp.x();
		fp.setX( sp.x() );
		sp.setX(tempX);
	}
	else if( ( fp.x() > sp.x() ) && ( fp.y() > sp.y() ) )
	{
		int tempX = fp.x();
		int tempY = fp.y();
		fp.setX( sp.x() );
		fp.setY( sp.y() );
		sp.setX(tempX);
		sp.setY(tempY);
	}
}


void FramesHandlerKernel::startPlayVideo()
{
	if( !m_capture.isOpened() ) return;
	double rate = m_capture.get(CV_CAP_PROP_FPS);
	int delay = 1000 / rate;

	cv::Mat frame;
	int frameCounter = 0;
	while( (!m_stopped) && (!m_paused) )
	{
		if( !m_capture.read(frame) ) break;

		m_timeCounter.start();
		formationImage(frame); //при обработке пятен в видео режиме могут возникнуть проблемы из-за асинхронности
						       //возможно стоит хранить ссылку на обработчик и делать эту операцию синхронно
							   //(т.е пока кадр не обработали дальше не идкм вообще) (требует теста)
		emit setCurrentMovePos( (int)( m_capture.get(CV_CAP_PROP_POS_FRAMES) ) );
		m_timeCounter.stop();

		int newDelay = (m_timeCounter.getDuration() * 1000);
		newDelay = delay - newDelay;
		if(newDelay < 0) newDelay = 0;
		m_timeCounter.reset();
		
		QTime time;
		time.start();
		for(; time.elapsed() < delay;) { ; }
	}
	if(!m_paused) 
	{
		readFirstFrame();
		emit setCurrentMovePos( (int)0 );
	}
}

void FramesHandlerKernel::startCamera()
{
	if( m_capture.isOpened() )
		m_capture.release();
	m_capture = cv::VideoCapture(0);
	if( !m_capture.isOpened() ) return;
	cv::Mat frame;
	while(cameraMode)
	{
		m_capture >> frame;
		//m_timeCounter.start();
		formationImage(frame);
		//m_timeCounter.stop();
		if(cv::waitKey(30) >= 0) break; 
	}
}

bool FramesHandlerKernel::isStopped() const
{
	return m_stopped;
}

bool FramesHandlerKernel::isPaused() const
{
	return m_paused;
}

void FramesHandlerKernel::readFirstFrame()
{
	cv::Mat firstFrame;
	m_capture.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
	m_capture.read(firstFrame);
	m_capture.set(CV_CAP_PROP_POS_AVI_RATIO, 0);

	formationImage(firstFrame);
}

void FramesHandlerKernel::formationImage(const cv::Mat & origFrame)
{
#ifdef COLOR_IMAGE
	if(!cameraMode)
	{
#endif
		QImage origImage = QImage( (const unsigned char*)(origFrame.data),
									origFrame.cols, origFrame.rows, QImage::Format_RGB888 );
		emit setImage(origImage);
#ifdef COLOR_IMAGE
	}
	else
	{
		cv::Mat temp( origFrame.cols, origFrame.rows, origFrame.type() );
		try
		{
			cv:cvtColor(origFrame, temp, CV_BGR2GRAY);
			internalFormation(temp);
		}
		catch(...)
		{
			internalFormation(temp);
		}
	}
#endif
}

void FramesHandlerKernel::internalFormation(const cv::Mat &frame)
{
	QImage origImage = QImage( (const unsigned char*)(frame.data),
		frame.cols, frame.rows, frame.step, QImage::Format_Indexed8 );
	origImage.bits();
	emit setImage(origImage);
}