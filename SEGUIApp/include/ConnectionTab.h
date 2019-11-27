#pragma once

#include <QtCore/QString>
#include <QtWidgets/QWidget>

#include "ui_ConnectionTab.h"

#include "BookDepthModel.h"
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

		void setBookDepthModel(BookDepthModel* model) {
			connectionTab_.bookDepthView->setModel(model);
		}

		void setTabName(const QString& name) {
			connectionTab_.connectionLabel->setText(name);
		}

		void setRecentTradesModel(RecentTradesModel* model) {
			connectionTab_.recentTradesView->setModel(model);
		}

		void setBookDepthWidget() {
			//connectionTab_.rTopVLayout->addWidget(widget);
		}
	};

} // namespace SEGUIApp
