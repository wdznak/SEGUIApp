#pragma once

#include <QtWidgets/QMainWindow>

#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <QDialog>
#include <QDebug>

#include <SEConnection.h>

#include "ui_SEGUIApp.h"
#include "AvailableConnectionsDialog.h"
#include "ConnectionsManager.h"
#include "ConnectionsTabManager.h"

namespace SEGUIApp {

	class HelpMe : public QObject {
		Q_OBJECT
	public:
		int i;
	signals:
		void newMsg(int, const std::string&);
	};

	class SEGUIApp : public QMainWindow
	{
		Q_OBJECT
	private:
		Ui::SEGUIAppClass ui;
		AvailableConnectionsDialog* availableConnections_;
		ConnectionsManager* connectionsManager_;
		ConnectionsTabManager* connectionsTabManager_;
		seservice::SEConnection* seConnection_;
		QDialog* dialog;
		std::function<void(int, std::string, seservice::ErrorCode)> err;
		std::function<void(int, std::string)> msgCb;

		HelpMe* helpMe;

	public:
		SEGUIApp(QWidget *parent = Q_NULLPTR);
		~SEGUIApp();

	private:
		void openConnectionsListDialog();
		void openConnection(std::string exchangeName, int connectionId, std::string symbol);
		void postError(QString message) {
			QListWidgetItem* errorMessage = new QListWidgetItem(QIcon{ ":/SEGUIApp/Resources/Icons/1x/round-panorama_fish_eye-24px.png" }, message);
			ui.outputListView->addItem(errorMessage);
		}
	};

} // namespace SEGUIApp

Q_DECLARE_METATYPE(std::string)