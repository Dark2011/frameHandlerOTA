#pragma once

//qt
#include <QtGui/QFrame>
#include "FLSAlgorithm.h"

class QTextEdit;
class QVBoxLayout;

class FramesHandlerResultWgt : public QFrame
{
	Q_OBJECT

public:
	FramesHandlerResultWgt(QWidget *parent = 0);
	void writeResult(const HandleResult &);
	void writeEndOfHandle();

public slots:
	void clearResults();

private:
	QVBoxLayout *resMainLayout;
	QTextEdit *frameHandlerResTE;

	QString resultString;
};