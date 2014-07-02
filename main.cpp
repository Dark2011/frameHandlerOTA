#include "frameshandlerMW.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	FrameHandlerMWindow frH;
	frH.show();

	return app.exec();
}
