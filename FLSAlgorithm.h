#pragma once
//qt
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtCore/QRect>
#include <QtCore/QQueue>

class QPoint;
class ShootPoint;

namespace FLS
{
	typedef QVector<ShootPoint*> ShootPoints;
	typedef QVector< QVector<ShootPoint*>* > Shoots;
	typedef QList< QVector<ShootPoint*>* > ResultsOfShooting;
	bool compareBy_X(const ShootPoint *, const ShootPoint *);
	bool compareBy_Y(const ShootPoint *, const ShootPoint *);
	bool compareBy_XY(const ShootPoint *, const ShootPoint *);
}

struct HandleResult 
{
	HandleResult(QObject *parent = 0) :	m_numOfFrame(-1), m_x(-1), m_y(-1),
									    m_br(-1), m_pixels(-1) { ; }
	
	int m_numOfFrame;
	int m_x;
	int m_y;
	int m_br;
	int m_pixels;
};


class FLSAlgorithm : public QObject
{
	Q_OBJECT

public:
	FLSAlgorithm(QObject *parent = 0);
	virtual ~FLSAlgorithm();

signals:
	void setFrameProperties(int, int);
	void sendResults(const HandleResult &);

public:
	void setResultFileName(const QString &);
	void setVideoFilePath(const QString &);
	void setNumOfFrame(int);

public slots:
	virtual void frameHandle() = 0; 
	virtual void reset();


	virtual void setFrame(const QPixmap &);
	virtual void setRoi(QPoint &, QPoint &);
	void setSensitivity(int);
	void setSizeFactor(int);
	void setThreshold(int);
	void setMBT(int);
	void calcFrameProperties();

	virtual void stopHandle();

protected:
	virtual void findAllShootsPixels();
	virtual void findAllShoots();
	virtual void finalFindShoots();

	virtual void insertNewShoot(ShootPoint &);
	virtual void savePrevBrightness(int, FLS::ShootPoints *);
	virtual bool brightnessFilter(int);

	void clearShootPoints();
	void clearShoots();

private:
	void checkRoiCoords(QPoint &, QPoint &);
	void calcMiddleBrigthess();
	void calcMiddleDynamicRange();

protected:
	QString m_resultFileName;
	QString m_videoFileName;
	int m_numFrame;
	
	QImage m_frame;
	QImage m_roiFrame;
	QRect m_roiRect;
	
	int m_middleBackgdBrightness;
	int m_middleDynamicRange;
	int minShootBrightness;
	int m_sensitivity;
	int m_sizeFactor;
	int m_threshold;
	int m_mbt;

	FLS::ShootPoints m_shootPoints;
	FLS::Shoots m_shoots;

	int m_prevLaserPointBrightness;
	int m_skipFramesCounter;
	QQueue<int>m_midPrevBrightness;

	bool firstPrintServiceData;
};


class FLSAlgorithm_OF_Type1 : public FLSAlgorithm //one frame
{
	Q_OBJECT

public:
	FLSAlgorithm_OF_Type1(QObject *parent = 0);

public slots:
	virtual void frameHandle();

protected:
	virtual void finalFindShoots();
};


class FLSAlgorithm_AF_Type1 : public FLSAlgorithm //all frames (video)
{
	Q_OBJECT

public:
	FLSAlgorithm_AF_Type1(QObject *parent = 0);

public slots:
	virtual void frameHandle(); 
};


class FLSAlgorithm_OF_Type2 : public FLSAlgorithm
{
	Q_OBJECT

public:
	FLSAlgorithm_OF_Type2(QObject *parent = 0);

public slots:
	virtual void frameHandle(); 

protected:
	virtual void findAllShootsPixels();
	virtual void findAllShoots();
	virtual void finalFindShoots();
	void simpleFindAllShoots();

private:
	void merge();

protected:
	FLS::Shoots m_finalShoots;
};


class FLSAlgorithm_OF_Type3 : public FLSAlgorithm_OF_Type2
{
	Q_OBJECT

public:
	FLSAlgorithm_OF_Type3(QObject *parent = 0);

public slots:
	virtual void frameHandle();
	virtual void setFrame(const QPixmap &);
	virtual void setRoi(QPoint &, QPoint &);
	virtual void reset();
	virtual void stopHandle();

protected:
	virtual void findAllShootsPixels();
	void substractionOfFrames(); 
	bool checkCorrectly(); //test
	void calcMinShootBr();

protected:
	bool firstFrame;
	bool frameWithShoot;
	QImage firstOrigFrame;
	QImage currSubFrame;
	QImage curProcFrame;
	QImage resultFrame;

	bool setup;
	QVector<int> tempArr;
};


class FLSAlgorithm_AF_Type2 : public FLSAlgorithm_OF_Type3 
{
	Q_OBJECT

public:
	FLSAlgorithm_AF_Type2(QObject *parent = 0);

//public slots:
	//virtual void frameHandle();
	
protected:
	virtual void finalFindShoots();
};



			