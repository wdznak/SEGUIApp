#pragma once

#include <QtCore/QDebug>

#include <string>

#include "ConnectionTab.h"
#include "ModelsManager.h"
#include "ui_ConnectionsTabWidget.h"

#include "BookDepthModel.h"
#include "FileDataParser.h"

namespace SEGUIApp {

	class ConnectionsTabManager : public QWidget
	{
		Q_OBJECT
	private:
		Ui::ConnectionsTabWidget connectionsTab_;
		bool isStartTabActive{ true };

		RecentTradesModel recentModel;

	public:
		ConnectionsTabManager();
		~ConnectionsTabManager();

		void addNewTab(int uniqueId, const QString& exchangeName, const QString& symbol, const Models* models) {

			/* --------  Delete This!  -------- */
			//BookDepthModel* bdm = new BookDepthModel();
			//FileDataParser* fdp = new FileDataParser(*bdm);
			/* --------       !!!      -------- */
			if (!models) {
				// Show error popup
				return;
			}
			
			ConnectionTab* connectionTab = new ConnectionTab();
			connectionTab->setProperty("uniqueId", QVariant(uniqueId));
			connectionTab->setProperty("exchangeName", QVariant(exchangeName));
			connectionTab->setTabName(exchangeName + " " + symbol);
			connectionTab->setRecentTradesModel(models->recentTrades.get());

			/* ------ Remove this ------ */
			//BookDepthHBC* bd = new BookDepthHBC();
			//connectionTab->setBookDepthWidget();
			/* ------     End     ------ */

			int index = connectionsTab_.tabWidget->addTab(connectionTab, symbol);
			connectionsTab_.tabWidget->setCurrentIndex(index);
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
