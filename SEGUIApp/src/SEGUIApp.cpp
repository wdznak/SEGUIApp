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

		ui.mainVLayout->addWidget(connectionsTabManager_.get());
		
		availableConnDialog_ = std::make_unique<AvailableConnectionsDialog>(this);
		availableConnDialog_->setModelData(connectionsManager_->availableConnections());

		connect(connectionsManager_.get(), &ConnectionsManager::error, this, &SEGUIApp::postError);
		connect(ui.actionOpen, &QAction::triggered, this, &SEGUIApp::openConnectionsListDialog);
		connect(connectionsManager_.get(), &ConnectionsManager::connectionOpened, connectionsTabManager_.get(), &ConnectionsTabManager::addNewTab);
		connect(availableConnDialog_.get(), &AvailableConnectionsDialog::connectionClicked, this, &SEGUIApp::openConnection);
		connect(ui.outputListView->model(), &QAbstractItemModel::rowsInserted, ui.outputListView, &QListWidget::scrollToBottom);
		connect(connectionsTabManager_.get(), &ConnectionsTabManager::tabClosed, connectionsManager_.get(), &ConnectionsManager::closeConnection);
	}

	SEGUIApp::~SEGUIApp()
	{
		delete connectionsTabManager_;
	}

	void SEGUIApp::openConnectionsListDialog()
	{
		availableConnDialog_->exec();
	}

	void SEGUIApp::openConnection(std::string exchangeName, int connectionId, std::string symbol)
	{
		connectionsManager_->createConnection(exchangeName, connectionId, symbol);
	}

} // namespace SEGUIApp