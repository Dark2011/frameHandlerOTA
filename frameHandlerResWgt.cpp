//own
#include "frameHandlerResWgt.h"
//qt
#include <QtGui/QLayout>
#include <QtGui/QTextEdit>
#include <QtCore/QDebug>

FramesHandlerResultWgt::FramesHandlerResultWgt(QWidget *parent) : QFrame(parent),
resultString( QString() )
{
	resMainLayout = new QVBoxLayout(this);
	frameHandlerResTE = new QTextEdit(this);
	frameHandlerResTE->setReadOnly(true);
	resMainLayout->addWidget(frameHandlerResTE);
	setFrameShape(QFrame::StyledPanel);
	setFrameShadow(QFrame::Sunken);
	setMaximumWidth(300);
	setLayout(resMainLayout);
}

void FramesHandlerResultWgt::writeResult(const HandleResult &result)
{
	QString numFrame = QString("Frame: %1 \n").arg(result.m_numOfFrame);
	QString coord_x = QString("Coordinate X: %1 \n").arg(result.m_x);
	QString coord_y = QString("Coordinate Y: %1 \n").arg(result.m_y);
	QString brightness = QString("Brightness: %1 \n").arg(result.m_br);
	QString pixelsCount = QString("Pixel in shoot: %1 \n\r").arg(result.m_pixels);
	QString res = numFrame + coord_x + coord_y + brightness + pixelsCount;

	resultString += res;
	frameHandlerResTE->insertPlainText(resultString);
	resultString = QString();
}

void FramesHandlerResultWgt::writeEndOfHandle()
{
	resultString = "------------------------------------------------------------\n\r\n\r";
	frameHandlerResTE->insertPlainText(resultString);
	resultString = QString();
}

void FramesHandlerResultWgt::clearResults()
{
	frameHandlerResTE->clear();
}
