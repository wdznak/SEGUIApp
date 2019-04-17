#include "AvailableConnectionsDialog.h"

#include <QDebug>

namespace SEGUIApp {

	AvailableConnectionsDialog::AvailableConnectionsDialog(QWidget* widget)
		: QDialog(widget)
	{
		availableConnections_.setupUi(this);

		connect(availableConnections_.listWidget, &QListWidget::itemDoubleClicked, this, &AvailableConnectionsDialog::connectionChosen);
	}

	AvailableConnectionsDialog::~AvailableConnectionsDialog()
	{
	}

	void AvailableConnectionsDialog::connectionChosen(QListWidgetItem* item) {
		std::string exchangeName = item->data(ConnectionListItem::Role::ExchangeNameRole).toString().toStdString();
		int connectionId = item->data(ConnectionListItem::Role::ConnectionIdRole).toInt();
		std::string symbol = item->data(ConnectionListItem::Role::SymbolRole).toString().toStdString();

		emit connectionClicked(exchangeName, connectionId, symbol);
	}

	void AvailableConnectionsDialog::setModelData(const model_t& data) {
		for (auto& exchange : data) {
			const QString& exchangeName = exchange.first.data();
			QListWidgetItem* item = new QListWidgetItem(exchangeName, availableConnections_.listWidget);
			item->setFlags(Qt::NoItemFlags);
			item->setForeground(QBrush{ Qt::lightGray });
			item->setBackground(QBrush{ Qt::lightGray });
			item->setTextColor(QColor{ Qt::red });

			for (auto& connection : exchange.second) {
				auto[connectionId, description, symbol] = connection;
				new ConnectionListItem(exchangeName, connectionId, description.data(), symbol.data(), availableConnections_.listWidget);
			}
		}
	}

} // namespace SEGUIApp
