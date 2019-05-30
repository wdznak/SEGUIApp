#pragma once

#include <QString>
#include <QWidget>

#include "ui_ConnectionTab.h"

#include "RecentTradesModel.h"

namespace SEGUIApp {

	class ConnectionTab: public QWidget
	{
	private:
		Ui::ConnectionTab connectionTab_;

	public:
		ConnectionTab(QWidget *parent = nullptr)
			: QWidget(parent)
		{
			connectionTab_.setupUi(this);
		}
		~ConnectionTab() = default;

		void setTabName(const QString& name) {
			connectionTab_.connectionLabel->setText(name);
		}

		void setRecentTradesModel(RecentTradesModel* model) {
			connectionTab_.recentTradesView->setModel(model);
		}
	};

} // namespace SEGUIApp
