#include "SEGUIApp.h"

#include <tuple>
#include <variant>

#include <QtWidgets/QAction>
#include <QtWidgets/QListWidget>

#include <uWS/uWS.h>

namespace SEGUIApp {

	SEGUIApp::SEGUIApp(QWidget *parent)
		: QMainWindow(parent)
	{
		qRegisterMetaType < std::string >();
		ui.setupUi(this);

		connectionsTabManager_ = std::make_unique<ConnectionsTabManager>();
		connectionsManager_ = std::make_unique<ConnectionsManager>(&modelsManager_);
		liveModeWidget_ = std::make_unique<LiveModeWidget>(connectionsManager_.get());
		offlineModeWidget_ = std::make_unique<OfflineModeWidget>();

		ui.stackedWidget->addWidget(offlineModeWidget_.get());
		ui.stackedWidget->addWidget(liveModeWidget_.get());

		availableConnDialog_ = std::make_unique<AvailableConnectionsDialog>(this);
		availableConnDialog_->setModelData(connectionsManager_->availableConnections());
		
		connect(ui.actionOpen, &QAction::triggered, this, &SEGUIApp::openConnectionsListDialog);	
		connect(availableConnDialog_.get(), &AvailableConnectionsDialog::connectionClicked, this, &SEGUIApp::openConnection);
		connect(ui.actionMode, &QAction::triggered, this, &SEGUIApp::switchMode);
	}

	void SEGUIApp::openConnectionsListDialog()
	{
		availableConnDialog_->exec();
	}

	void SEGUIApp::openConnection(std::string exchangeName, int connectionId, std::string symbol)
	{
		connectionsManager_->createConnection(exchangeName, connectionId, symbol);
	}

	void SEGUIApp::switchMode() {
		if (!liveMode) {
			ui.stackedWidget->setCurrentIndex(1);
			ui.actionMode->setText(tr("Live Mode"));
		}
		else {
			ui.stackedWidget->setCurrentIndex(0);
			ui.actionMode->setText(tr("Offline Mode"));
		}

		liveMode = !liveMode;
	}

} // namespace SEGUIApp