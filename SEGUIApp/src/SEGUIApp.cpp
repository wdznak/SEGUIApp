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

		connectionsTabManager_ = new ConnectionsTabManager();
		connectionsManager_ = new ConnectionsManager(&modelsManager_);

		ui.mainVLayout->addWidget(connectionsTabManager_);
		
		availableConnections_ = new AvailableConnectionsDialog{ this };
		availableConnections_->setModelData(connectionsManager_->availableConnections());

		connect(connectionsManager_, &ConnectionsManager::error, this, &SEGUIApp::postError);
		connect(ui.actionOpen, &QAction::triggered, this, &SEGUIApp::openConnectionsListDialog);
		connect(connectionsManager_, &ConnectionsManager::connectionOpened, connectionsTabManager_, &ConnectionsTabManager::addNewTab);
		connect(availableConnections_, &AvailableConnectionsDialog::connectionClicked, this, &SEGUIApp::openConnection);
		connect(ui.outputListView->model(), &QAbstractItemModel::rowsInserted, ui.outputListView, &QListWidget::scrollToBottom);
		connect(connectionsTabManager_, &ConnectionsTabManager::tabClosed, connectionsManager_, &ConnectionsManager::closeConnection);
	}

	SEGUIApp::~SEGUIApp()
	{
		delete connectionsTabManager_;
		delete connectionsManager_;
		delete availableConnections_;
	}

	void SEGUIApp::openConnectionsListDialog()
	{
		availableConnections_->exec();
	}

	void SEGUIApp::openConnection(std::string exchangeName, int connectionId, std::string symbol)
	{
		connectionsManager_->createConnection(exchangeName, connectionId, symbol);
	}

} // namespace SEGUIApp