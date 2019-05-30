#include "SEGUIApp.h"

#include <string>

#include <QtWidgets/QApplication>

#include "DispatchI.h"

//Q_DECLARE_METATYPE(std::string);
	
int main(int argc, char *argv[])
{
	qRegisterMetaType<SEGUIApp::Trade>("Trade");
	QApplication a(argc, argv);
	SEGUIApp::SEGUIApp w;
	w.show();
	return a.exec();
}
