#pragma once

#include <QDebug>

#include <string>

#include "ui_ConnectionTab.h"
#include "ui_ConnectionsTabWidget.h"

namespace SEGUIApp {

	class ConnectionsTabManager : public QWidget
	{
		Q_OBJECT
	private:
		Ui::ConnectionsTabWidget connectionsTab_;
		bool isStartTabActive{ true };

	public:
		ConnectionsTabManager();
		~ConnectionsTabManager();

		void addNewTab(int uniqueId, const QString& exchangeName,const QString& symbol) {
			QWidget* newTab = new QWidget();
			newTab->setProperty("uniqueId", QVariant(uniqueId));
			newTab->setProperty("exchangeName", QVariant(exchangeName));
			Ui::ConnectionTab* connectionTab = new Ui::ConnectionTab();
			connectionTab->setupUi(newTab);
			connectionTab->connectionLabel->setText(exchangeName + " " + symbol);
			connectionsTab_.tabWidget->addTab(newTab, symbol);
		}

	private:
		void addStartTab();
		void removeStartTab(int index);

	signals:
		void tabClosed(int index, std::string exchangeName);

	private slots:
		void onTabCloseRequested(int index);
	};

} // namespace SEGUIApp
