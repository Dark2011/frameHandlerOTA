#pragma once

#include <QtGui/QPixmap>
#include <QtGui/QLabel>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QTimer>

QT_FORWARD_DECLARE_CLASS(QPaintEvent);
QT_FORWARD_DECLARE_CLASS(QString);
QT_FORWARD_DECLARE_CLASS(QSize);
QT_FORWARD_DECLARE_CLASS(QMouseEvent);

class PixmapLabel : public QLabel
{
	Q_OBJECT

public:
	PixmapLabel(QWidget *parent = nullptr);

	bool overScalingEnable() const;
	bool keepAspectRatioEnable() const;
	QSize getOriginalSize() const;

protected:
	virtual void paintEvent(QPaintEvent *);

public slots:
	void setScaledPixmap(const QPixmap &);
	void loadPixmap(const QString &);
	void removePixmap();
	void enableOverScaling(bool enable = true);
	void enableKeepAspectRatio(bool enable = true);
	void setFrame(bool flag = true);
	const QString& getPixmapPath() const; 

	const QPixmap& getFrame() const;

private:
	bool fitToScreen(const QSize &);

protected:
	QPixmap scaledPixmap;

private:
	QPixmap m_pixmap;
	QString m_pixmapPath;
	bool m_keepAspectRatio;
	bool m_overScalingEnabled;
	QSize m_originalSize;
};

class PixmapRoiLabel : public PixmapLabel
{
	Q_OBJECT

public:
	PixmapRoiLabel(QWidget *parent = 0);
	bool roiSaved() const;

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void resizeEvent(QResizeEvent *);

signals:
	void setRoi(QPoint &, QPoint &);
	void clearRoiRect();
	void setCurrentCursorPos(const QPoint &);

public slots:
	void drawROI(bool);
	void drawCurrentROI(bool);
	void saveROI(bool, bool = true);
	void clearRoi();
	void drawCornerPoint(const QString &);
	void notDrawCorners();

private:
	void calcRoiScale();
	bool isRoiRectSet();

private:
	QPoint firstROICoord;
	QPoint secondROICoord;
	QPoint firstRoiRectCoord;
	QPoint secondRoiRectCoord;

	bool enableDrawRect;
	bool startDrawRect;
	bool savedRect;
	bool drawSaveText;
	bool drawCornPoint;
	bool currentDraw;

	QString m_cornerName;

	QTimer updateTimer;
};