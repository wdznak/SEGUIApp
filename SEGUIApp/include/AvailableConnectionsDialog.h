#pragma once
#include <QtWidgets/QDialog>
#include "ui_AvailableConnections.h"

#include <tuple>

#include <QtCore/QDebug>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtCore/QString>

namespace SEGUIApp {
	namespace {

		class ConnectionListItem : public QListWidgetItem
		{
		private:
			int connectionId_;
			QString exchangeName_;
			QString symbol_;

		public:
			enum Role : int {
				ConnectionIdRole = 1000,
				ExchangeNameRole,
				SymbolRole
			};

			explicit ConnectionListItem(const QString& exchangeName, int connectionId, const QString& symbol, const QString &text, QListWidget *view)
				: exchangeName_(exchangeName), connectionId_(connectionId), symbol_(symbol), QListWidgetItem(text, view)
			{}

			QVariant data(int role) const override {
				if (Role::ConnectionIdRole == role) {
					return QVariant{ connectionId_ };
				}
				else if (Role::ExchangeNameRole == role) {
					return exchangeName_;
				}
				else if (Role::SymbolRole == role) {
					return symbol_;
				}

				return QListWidgetItem::data(role);
			}
		};

	} // namespace

	class AvailableConnectionsDialog : public QDialog
	{
	private:
		Q_OBJECT
			Ui::AvailableConnectionsDialog availableConnections_;

	public:
		using model_t = std::map<std::string, std::vector<std::tuple<int, std::string, std::string>>>;
		AvailableConnectionsDialog(QWidget* widget);
		~AvailableConnectionsDialog();

		void connectionChosen(QListWidgetItem* item);
		void setModelData(const model_t& data);

	signals:
		void connectionClicked(std::string, int, std::string);

	private:
		//friend class AvailableConnDialogTest;
	};

} // namespace SEGUIApp
