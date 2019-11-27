#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <variant>

#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtWidgets/QWidget>

#include <tbb/concurrent_queue.h>

#include <Connection.h>
#include <UvFileStream.h>

#include "Broadcaster.h"
#include "MessageDispatch.h"
#include "ModelsManager.h"

namespace SEGUIApp {

	class ConnectionsManager: public QObject
	{
		Q_OBJECT
	public:
		using connList_t = seservice::Connection::connList_t;
		
	private:
		struct Error {
			int id;
			std::string message;
			std::variant<seservice::ErrorCode, sefile::ErrorCode> errorCode;
		};

		struct Connection {
			int id;
			std::string exchangeName;
			std::string symbol;
		};

		struct ActiveConnection {
			int connectionId;
			int fileStreamId;
			Models* models;
		};
		
		tbb::concurrent_queue<Error> errorQueue;
		seservice::Connection connectionService_{ [&](int connectionId, std::string message, seservice::ErrorCode errorCode) {
			errorQueue.push(Error{connectionId, message, errorCode});
			// TODO: Move errorMessage to errorHandler() and make error signal with no arguments.
			QString errorMessage = QString("Connection nr. %1 has failed. Error message: %2").arg(connectionId).arg(message.data());
			emit error(errorMessage);
		} };

		sefile::UvFileStream fileStream_{ [&](int streamId, std::string message, sefile::ErrorCode errorCode) {
			errorQueue.push(Error{streamId, message, errorCode});
			// TODO: Move errorMessage to errorHandler() and make error signal with no arguments.
			QString errorMessage = QString("File stream nr. %1 has failed. Error message: %2").arg(streamId).arg(message.data());
			emit error(errorMessage);
		} };
		std::queue<Connection> awaitConnectionQueue_;
		std::vector<ActiveConnection> activeConnectionList_;
		std::thread loopThread_;
		MessageDispatch dispatch_;
		ModelsManager* modelsManager_;

	public:
		ConnectionsManager(ModelsManager* modelsManager = nullptr);
		~ConnectionsManager();

		connList_t availableConnections() {
			return connectionService_.availableConnections();
		}

		void closeConnection(int uniqueId, const std::string& exchangeName) {
			connectionService_.close(uniqueId, exchangeName);
		}

		void createConnection(const std::string& exchangeName, int connectionId, const std::string& symbol);
		void errorHandler(QString message);
		void fileStreamMessageHandler(int id, size_t size);

	signals:
		void connectionClosed(int);
		void connectionOpened(int, QString, QString, const Models*);
		void error(QString);
		void fileStreamOpened(int, QPrivateSignal);
		void fileSizeUpdated(int, size_t);

	private:
		void close(int connectionId);
		void closeAll();
		void connectAll(int fileStreamId);

	};

} // namespace SEGUIApp
