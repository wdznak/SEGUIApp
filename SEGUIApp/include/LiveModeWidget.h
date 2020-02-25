#pragma once

#include <QWidget>
#include <QtCore/QString>

#include "ui_LiveModeWidget.h"

#include "ConnectionsTabManager.h"
#include "ConnectionsManager.h"

namespace SEGUIApp {

	class LiveModeWidget : public QWidget
	{
		Q_OBJECT
	private:
		Ui::LiveModeWidget liveMode_;
		std::unique_ptr<ConnectionsTabManager> connectionsTabManager_;

	public:
		LiveModeWidget(ConnectionsManager* connectionsManager) {
			liveMode_.setupUi(this);
			liveMode_.splitter->setSizes(QList<int>{(int)(QWidget::height()*0.8), (int)(QWidget::height()*0.2)});
			
			connectionsTabManager_ = std::make_unique<ConnectionsTabManager>();
			liveMode_.mainVLayout->addWidget(connectionsTabManager_.get());

			connect(connectionsTabManager_.get(), &ConnectionsTabManager::tabClosed, connectionsManager, &ConnectionsManager::closeConnection);
			connect(connectionsManager, &ConnectionsManager::connectionOpened, connectionsTabManager_.get(), &ConnectionsTabManager::addNewTab);
			connect(connectionsManager, &ConnectionsManager::error, this, &LiveModeWidget::postError);
			connect(liveMode_.outputListView->model(), &QAbstractItemModel::rowsInserted, liveMode_.outputListView, &QListWidget::scrollToBottom);
		}

		void postError(QString message) {
			QListWidgetItem* errorMessage = new QListWidgetItem(QIcon{ ":/SEGUIApp/Resources/Icons/1x/round-panorama_fish_eye-24px.png" }, message);
			liveMode_.outputListView->addItem(errorMessage);
		}
	};

} // namespace SEGUIApp
