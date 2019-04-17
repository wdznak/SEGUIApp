#include "ConnectionsManager.h"

namespace SEGUIApp {

	ConnectionsManager::ConnectionsManager()
	{
		loopThread_ = std::thread(&sefile::UvFileStream::runLoop, std::ref(fileStream_));
		connect(this, &ConnectionsManager::error, this, &ConnectionsManager::errorHandler, Qt::QueuedConnection);
		connect(this, &ConnectionsManager::fileStreamOpened, this, &ConnectionsManager::connectAll, Qt::QueuedConnection);
	}


	ConnectionsManager::~ConnectionsManager()
	{
		closeAll();
	}

} // namespace SEGUIApp
