//own
#include "pixmapLabel.h"
//qt
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QMouseEvent>
#include <QtGui/QBrush>
#include <QtCore/QDebug>
#include <QtGui/QFont>

const QString TLCorner = QObject::tr("Top left corner");
const QString TRCorner = QObject::tr("Top right corner");
const QString BLCorner = QObject::tr("Bottom left corner");
const QString BRCorner = QObject::tr("Bottom right corner");
const QString AllCorners = QObject::tr("All corners");

PixmapLabel::PixmapLabel(QWidget *parent) : QLabel(parent),
m_overScalingEnabled(true), m_keepAspectRatio(true), m_pixmapPath(""){ ; }

/*virtual*/void PixmapLabel::paintEvent(QPaintEvent *pe)
{
	QPainter painter(this);
	if( !m_pixmap.isNull() )
	{
		QPoint centerPoint(0, 0);
		QSize scaledSize = overScalingEnable() ? size() : 
			               ( fitToScreen( size() ) ? m_originalSize : size() );

		/*QPixmap*/ scaledPixmap = m_pixmap.scaled(scaledSize, m_keepAspectRatio ?
			                                   Qt::KeepAspectRatio : Qt::IgnoreAspectRatio);
		
		centerPoint.setX( ( size().width() - scaledPixmap.width() ) / 2 );
		centerPoint.setY( ( size().height() - scaledPixmap.height() ) / 2 );
		
		painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
		//QPainter testPainter(&scaledPixmap); //можно выводить суммарное число кадров
		//testPainter.drawText(30, 30, "Test");
		painter.drawPixmap(centerPoint, scaledPixmap);

		
	}
	//else painter.drawPixmap(QPoint(0,0), m_pixmap);
	QLabel::paintEvent(pe);
}

/*slot*/void PixmapLabel::setScaledPixmap(const QPixmap &pixmap)
{
	m_pixmap = pixmap;
	m_originalSize = pixmap.size();
	update();
}

/*slot*/void PixmapLabel::loadPixmap(const QString &path)
{
	QPixmap pixmap(path);
	if( !pixmap.isNull() )
	{
		m_pixmapPath = path;
		setScaledPixmap(pixmap);
	}
	else removePixmap();
}

/*slot*/void PixmapLabel::setFrame(bool flag)
{
	if(flag)
	{
		setFrameShape(QFrame::StyledPanel);
		setFrameShadow(QFrame::Sunken);
	}
	else
	{
		setFrameShape(QFrame::NoFrame);
		setFrameShadow(QFrame::Plain);
	}
	update();
}

bool PixmapLabel::overScalingEnable() const
{
	return m_overScalingEnabled;
}

bool PixmapLabel::keepAspectRatioEnable() const
{
	return m_keepAspectRatio;
}

QSize PixmapLabel::getOriginalSize() const
{
	return m_originalSize;
}

/*slot*/void PixmapLabel::enableOverScaling(bool enable)
{
	m_overScalingEnabled = enable;
	update();
}

/*slot*/void PixmapLabel::enableKeepAspectRatio(bool enable)
{
	m_keepAspectRatio = enable;
	update();
}

bool PixmapLabel::fitToScreen(const QSize &screenSize)
{
	return ( ( screenSize.width() >= m_originalSize.width() ) &&
		     ( screenSize.height() >= m_originalSize.height() ) );
}

void PixmapLabel::removePixmap()
{
	this->clear();
	m_pixmap = 0;
	m_pixmapPath = "";
	update();
}

const QString& PixmapLabel::getPixmapPath() const
{
	return m_pixmapPath;
}

const QPixmap& PixmapLabel::getFrame() const
{
	return m_pixmap;
}


PixmapRoiLabel::PixmapRoiLabel(QWidget *parent) : PixmapLabel(parent),
firstROICoord(0, 0), secondROICoord(0, 0), firstRoiRectCoord(0, 0), secondRoiRectCoord(0, 0),
enableDrawRect(true), startDrawRect(false), drawSaveText(false),
savedRect(false), drawCornPoint(false), currentDraw(false), m_cornerName("")
{
	setMouseTracking(true);
	updateTimer.setInterval(5000);
	connect(&updateTimer, SIGNAL( timeout() ), this, SLOT( repaint() ) );
}

bool PixmapRoiLabel::roiSaved() const
{
	return savedRect;
}

/*virtual*/void PixmapRoiLabel::paintEvent(QPaintEvent *pe)
{
	PixmapLabel::paintEvent(pe);
	QPainter painter(this);
	
	if(drawSaveText)
	{
		QPen textPen(QColor(Qt::red), 5);
		textPen.setStyle(Qt::SolidLine);
		textPen.setWidth(3);
		painter.setPen(textPen);
		QFont textFont;
		textFont.setPixelSize(20);
		painter.setFont(textFont);
		painter.drawText(30, 30, "ROI rect was saved");
		drawSaveText = false;
		updateTimer.start();
	}
	QPen rectPen(QColor(Qt::yellow), 2);
	painter.setPen(rectPen);
	if(currentDraw) painter.drawRect( QRect(firstRoiRectCoord, secondRoiRectCoord) );
	if( enableDrawRect && isRoiRectSet() )
	{
		//qDebug() << this->size();
		//qDebug() << scaledPixmap.rect();
		//qDebug() << getOriginalSize();

		painter.drawRect( QRect(firstRoiRectCoord, secondRoiRectCoord) );
	}

	if(drawCornPoint)
	{
		QPen pointPen(QColor(Qt::red), 1);
		QBrush pointBrush( QColor(255, 0, 0) );
		painter.setPen(pointPen);
		painter.setBrush(pointBrush);
		QRect rect(firstRoiRectCoord, secondRoiRectCoord);
		QPoint corner;
		if(m_cornerName != AllCorners)
		{
			if(m_cornerName == TLCorner) corner = rect.topLeft(); 
			else if(m_cornerName == TRCorner) corner = rect.topRight();
			else if(m_cornerName == BLCorner) corner = rect.bottomLeft();
			else if(m_cornerName == BRCorner) corner = rect.bottomRight();
			painter.drawEllipse(corner, 4, 4);
		}
		else
		{
			painter.drawEllipse(rect.topLeft(), 4, 4);
			painter.drawEllipse(rect.topRight(), 4, 4);
			painter.drawEllipse(rect.bottomLeft(), 4, 4);
			painter.drawEllipse(rect.bottomRight(), 4, 4);
		}
	}
}

void PixmapRoiLabel::drawROI(bool flag)
{
	enableDrawRect = flag;
	repaint();
	//if(!flag) clearRoi();
}

void PixmapRoiLabel::drawCurrentROI(bool flag)
{
	currentDraw = flag;
	repaint();
}

void PixmapRoiLabel::saveROI(bool flag, bool showMessage)
{
	currentDraw = false;
	savedRect = flag;
	drawSaveText = showMessage;
	emit setRoi(firstROICoord, secondROICoord); 
	repaint();
}

/*virtual*/void PixmapRoiLabel::mousePressEvent(QMouseEvent *mpe)
{
	if(currentDraw)
	{
		int diff_X = ( width() - scaledPixmap.width() ) / 2;
		int diff_Y = ( height() - scaledPixmap.height() ) / 2;
		firstROICoord.setX(mpe->x() - diff_X);
		firstROICoord.setY(mpe->y() - diff_Y);
		firstRoiRectCoord.setX( mpe->x() );
		firstRoiRectCoord.setY( mpe->y() );

		//qDebug() << (mpe->x() - diff_X) << (mpe->y() - diff_Y);
		//qDebug() << scaledPixmap.width() << scaledPixmap.height();

		PixmapLabel::mousePressEvent(mpe);
		startDrawRect = true;
	}
}

/*virtual*/void PixmapRoiLabel::mouseMoveEvent(QMouseEvent *mme)
{
	if(currentDraw)
	{
		int diff_X = ( width() - scaledPixmap.width() ) / 2;
		int diff_Y = ( height() - scaledPixmap.height() ) / 2;
	
		if(startDrawRect)
		{
			secondROICoord.setX(mme->x() - diff_X);
			secondROICoord.setY(mme->y() - diff_Y);
			secondRoiRectCoord.setX( mme->x() );
			secondRoiRectCoord.setY( mme->y() );
			//emit setRoi(firstROICoord, secondROICoord); //test
			repaint();
		}
		PixmapLabel::mouseMoveEvent(mme);
		emit setCurrentCursorPos(QPoint( (mme->x() - diff_X), (mme->y() - diff_Y) ) );
		//qDebug() << (mme->x() - diff_X) << (mme->y() - diff_Y);
	}
}

/*virtual*/void PixmapRoiLabel::mouseReleaseEvent(QMouseEvent *mre)
{	
	if(currentDraw)
	{
		startDrawRect = false;
		PixmapLabel::mouseReleaseEvent(mre);
		calcRoiScale();
		//emit setRoi(firstROICoord, secondROICoord); 
	}
	//PixmapLabel::mouseReleaseEvent(mre);
}

void PixmapRoiLabel::calcRoiScale()
{
	//qDebug() << getOriginalSize();
	//qDebug() << scaledPixmap.size();

	float kx = (float)getOriginalSize().width() / (float)scaledPixmap.width();
	float ky = (float)getOriginalSize().height() / (float)scaledPixmap.height();

	if(kx != 1.0 || ky != 1.0)
	{
		float tempX1 = (float)firstROICoord.x() * kx;
		float tempY1 = (float)firstROICoord.y() * ky;
		float tempX2 = (float)secondROICoord.x() * kx;
		float tempY2 = (float)secondROICoord.y() * ky;

		firstROICoord.setX(tempX1);
		firstROICoord.setY(tempY1);
		secondROICoord.setX(tempX2);
		secondROICoord.setY(tempY2);
	}
}

/*virtual*/void PixmapRoiLabel::resizeEvent(QResizeEvent *re)
{
	qDebug() << "Old size: " << re->oldSize() << "  New size: " <<  re->size();
	clearRoi();
	//saveROI(false);
	emit clearRoiRect();
}

void PixmapRoiLabel::clearRoi()
{
	firstRoiRectCoord.setX(0);
	firstRoiRectCoord.setY(0);
	secondRoiRectCoord.setX(0);
	secondRoiRectCoord.setY(0);
	drawCornPoint = false;
	repaint();
}

void PixmapRoiLabel::drawCornerPoint(const QString &cornerName)
{
	drawCornPoint = true;
	m_cornerName = cornerName;
	repaint();
}

void PixmapRoiLabel::notDrawCorners()
{
	drawCornPoint = false;
	repaint();
}

bool PixmapRoiLabel::isRoiRectSet()
{
	if( firstRoiRectCoord.isNull() && secondRoiRectCoord.isNull() ) return false;
	else return true;
}
