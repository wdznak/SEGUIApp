#include "ConnectionsTabManager.h"

#include <QMessageBox>

namespace SEGUIApp {

	ConnectionsTabManager::ConnectionsTabManager()
	{
		connectionsTab_.setupUi(this);
		connect(connectionsTab_.tabWidget, &QTabWidget::tabCloseRequested, this, &ConnectionsTabManager::onTabCloseRequested);
	}


	ConnectionsTabManager::~ConnectionsTabManager()
	{
		if (!isStartTabActive) {
			delete connectionsTab_.startTab;
		}
	}

	void ConnectionsTabManager::addStartTab() {
		if (!isStartTabActive) {
			connectionsTab_.tabWidget->insertTab(0, connectionsTab_.startTab, "Start page");
			isStartTabActive = true;
		}
	}

	void ConnectionsTabManager::removeStartTab(int index) {
		if (isStartTabActive) {
			connectionsTab_.tabWidget->removeTab(index);
			isStartTabActive = false;
			qDebug() << "Start Tab is removed!";
		}
	}

	// ----- Slots definitions -----

	void ConnectionsTabManager::onTabCloseRequested(int index) {
		if (QWidget* widget = connectionsTab_.tabWidget->widget(index)) {
			QVariant uniqueId;
			QVariant exchangeName;

			QMessageBox msgBox;
			QLabel* connectionLabel = widget->findChild<QLabel*>("connectionLabel");

			if (connectionLabel) msgBox.setText("Connection to " + connectionLabel->text() + " will be closed!");

			msgBox.setInformativeText("Do you want to continue?");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Cancel);
			int ret = msgBox.exec();

			if (QMessageBox::Cancel == ret) return;

			connectionsTab_.tabWidget->removeTab(index);
			uniqueId = widget->property("uniqueId");
			exchangeName = widget->property("exchangeName");
			delete widget;

			if (!connectionsTab_.tabWidget->count()) addStartTab();

			emit tabClosed(uniqueId.toInt(), exchangeName.toString().toStdString());
		}
	}

} // namespace SEGUIApp
