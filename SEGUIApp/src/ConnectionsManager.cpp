#include "ConnectionsManager.h"

#include "BinMessageDispatch.h"

namespace SEGUIApp {

	ConnectionsManager::ConnectionsManager(ModelsManager* modelsManager)
		: modelsManager_(modelsManager)
	{
		loopThread_ = std::thread(&sefile::UvFileStream::runLoop, std::ref(fileStream_));
		connect(this, &ConnectionsManager::error, this, &ConnectionsManager::errorHandler, Qt::QueuedConnection);
		connect(this, &ConnectionsManager::fileStreamOpened, this, &ConnectionsManager::connectAll, Qt::QueuedConnection);
	}


	ConnectionsManager::~ConnectionsManager()
	{
		closeAll();
	}

	void ConnectionsManager::createConnection(const std::string& exchangeName, int connectionId, const std::string& symbol) {
		awaitConnectionQueue_.push(Connection{ connectionId, exchangeName, symbol });
		std::string fileName = exchangeName + "_" + symbol;

		fileStream_.createFileStreamAsync(fileName, [&](int id, size_t size) {
			fileStreamMessageHandler(id, size);
		});
	}

	void ConnectionsManager::errorHandler(QString message) {
		Error error;
		if (!errorQueue.try_pop(error)) return;
		
		if (error.errorCode.index() == 0) {
			const seservice::ErrorCode& code = std::get<0>(error.errorCode);

			if (code == seservice::CONNECTIONCLOSED) {
				close(error.id);
			}
			else if (code == seservice::ECONNECTIONLOST) {
				qDebug() << "CONNETION LOST EVEN AFTER RETRY!";
			}
		}
		else if (error.errorCode.index() == 1) {
			qDebug() << "File stream service Error!";
		}
	}

	void ConnectionsManager::fileStreamMessageHandler(int id, size_t size) {
		if (size > -1) {
			emit fileSizeUpdated(id, size);
		}
		else if (size == -1) {
			emit fileStreamOpened(id, QPrivateSignal{});
		}
	}

	void ConnectionsManager::close(int connectionId)
	{
		auto it = std::find_if(activeConnectionList_.begin(), activeConnectionList_.end(), [&connectionId](const ActiveConnection& connection) {
			return connection.connectionId == connectionId;
		});

		if (it != activeConnectionList_.end()) {
			emit connectionClosed(it->connectionId);
			fileStream_.closeFileStreamAsync(it->fileStreamId);
			modelsManager_->removeModels(it->models);
		}
	}

	void ConnectionsManager::closeAll() {
		connectionService_.closeAll();
		fileStream_.stop();
		loopThread_.join();
	}
	
	void ConnectionsManager::connectAll(int fileStreamId) {
		if (awaitConnectionQueue_.empty()) return;

		const Connection& connection = awaitConnectionQueue_.front();
		auto dispatch = dispatch_.createDispatch(connection.exchangeName);

		if (!dispatch) {
			awaitConnectionQueue_.pop();
			return;
		}

		Models* models = modelsManager_->createModels(dispatch->getExchange());

		if (models) {
			switch (dispatch->getExchange()) {
			case Exchange::BINANCE : 
				QObject::connect(dispatch.get(), &DispatchI::tradeCreated, models, &Models::onNewTrade);
				break;
			}
		}
		
		int connectionId = connectionService_.connect(connection.exchangeName, connection.id, [&, dispatch{ dispatch }, fileStreamId](int connectionId, std::string message) {
			fileStream_.writeData(fileStreamId, message);
			dispatch->dispatch(connectionId, message);
		});

		emit connectionOpened(connectionId, connection.exchangeName.data(), connection.symbol.data(), models);
		activeConnectionList_.push_back(ActiveConnection{ connectionId, fileStreamId, models});
		awaitConnectionQueue_.pop();
	}

} // namespace SEGUIApp

