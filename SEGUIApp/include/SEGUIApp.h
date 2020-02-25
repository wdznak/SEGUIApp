#pragma once

#include <QtWidgets/QMainWindow>

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <QtWidgets/QDialog>
#include <QtCore/QDebug>

#include <Connection.h>

#include "ui_SEGUIApp.h"

#include "AvailableConnectionsDialog.h"
#include "ConnectionsManager.h"
#include "ConnectionsTabManager.h"
#include "LiveModeWidget.h"
#include "ModelsManager.h"
#include "OfflineModeWidget.h"

namespace SEGUIApp {

	class SEGUIApp : public QMainWindow
	{
		Q_OBJECT
	private:
		Ui::SEGUIAppClass ui;

		std::unique_ptr<AvailableConnectionsDialog> availableConnDialog_;
		std::unique_ptr<ConnectionsManager> connectionsManager_;
		std::unique_ptr<ConnectionsTabManager> connectionsTabManager_;
		std::unique_ptr<LiveModeWidget> liveModeWidget_;
		bool liveMode = true;
		ModelsManager modelsManager_;
		std::unique_ptr<OfflineModeWidget> offlineModeWidget_;

		std::function<void(int, std::string, seservice::ErrorCode)> err;
		std::function<void(int, std::string)> msgCb;


	public:
		SEGUIApp(QWidget *parent = Q_NULLPTR);
		~SEGUIApp() = default;

	private:
		void openConnectionsListDialog();
		void openConnection(std::string exchangeName, int connectionId, std::string symbol);
		void switchMode();
	};

} // namespace SEGUIApp

Q_DECLARE_METATYPE(std::string)