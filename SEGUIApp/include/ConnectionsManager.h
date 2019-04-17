#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <variant>

#include <QDebug>
#include <QObject>
#include <QWidget>

#include <tbb/concurrent_queue.h>

#include <SEConnection.h>
#include <UvFileStream.h>

#include "Broadcaster.h"

namespace SEGUIApp {

	class ConnectionsManager: public QObject
	{
		Q_OBJECT
	public:
		using connList_t = seservice::SEConnection::connList_t;

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
		
		tbb::concurrent_queue<Error> errorQueue;
		seservice::SEConnection connectionService_{ [&](int connectionId, std::string message, seservice::ErrorCode errorCode) {
			errorQueue.emplace(Error{connectionId, message, errorCode});
			QString errorMessage = QString("Connection nr. %1 has failed. Error message: %2").arg(connectionId).arg(message.data());
			emit error(errorMessage);
		} };

		sefile::UvFileStream fileStream_{ [&](int streamId, std::string message, sefile::ErrorCode errorCode) {
			errorQueue.emplace(Error{streamId, message, errorCode});
			QString errorMessage = QString("File stream nr. %1 has failed. Error message: %2").arg(streamId).arg(message.data());
			emit error(errorMessage);
		} };
		std::queue<Connection> awaitConnectionQueue_;
		std::thread loopThread_;

	public:
		ConnectionsManager();
		~ConnectionsManager();

		connList_t availableConnections() {
			return connectionService_.availableConnections();
		}

		void closeConnection(int uniqueId, const std::string& exchangeName) {
			connectionService_.close(uniqueId, exchangeName);
			qDebug() << "Closing connection nr: " << uniqueId;
		}

		void createConnection(const std::string& exchangeName, int connectionId, const std::string& symbol) {
			awaitConnectionQueue_.emplace(Connection{ connectionId, exchangeName, symbol });
			std::string fileName = exchangeName + "_" + symbol;

			fileStream_.createFileStreamAsync(fileName, [&](int id, size_t size) {
				fileStreamMessageHandler(id, size);
			});
		}
	
		void errorHandler(QString message) {
			Error error;
			if(!errorQueue.try_pop(error)) return;
			switch (error.errorCode.index()) {
				case 0: qDebug() << "Seservice Error!";
					break;
				case 1: qDebug() << "File stream service Error!";
					break;
			}
		}

		void fileStreamMessageHandler(int id, size_t size) {
				if( size > -1 ) {
					emit fileSizeUpdated(id, size);
				} else if( size == -1 ){
					emit fileStreamOpened(id, QPrivateSignal{});
				}
		}
	signals:
		void connectionClosed(int);
		void connectionOpened(int, QString, QString);
		void error(QString);
		void fileStreamOpened(int, QPrivateSignal);
		void fileSizeUpdated(int, size_t);

	private:
		void connectAll(int fileStreamId) {
			if (awaitConnectionQueue_.empty()) return;
			const Connection& connection = awaitConnectionQueue_.front();
			int connectionId = connectionService_.connect(connection.exchangeName, connection.id, [&, fileStreamId](int id, std::string message) {
				fileStream_.writeData(fileStreamId, message);
			});

			emit connectionOpened(connectionId, connection.exchangeName.data(), connection.symbol.data());
			awaitConnectionQueue_.pop();
		}

		void closeAll() {
			connectionService_.closeAll();
			fileStream_.stop();
			loopThread_.join();
		}
	};

} // namespace SEGUIApp
