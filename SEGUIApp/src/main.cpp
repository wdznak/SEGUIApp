#include "SEGUIApp.h"
#include <QtWidgets/QApplication>
#include <string>

//Q_DECLARE_METATYPE(std::string);
	
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SEGUIApp::SEGUIApp w;
	w.show();
	return a.exec();
}
