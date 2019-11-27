#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include <QtCore/QObject>


namespace SEGUIApp {

	class Broadcaster: public QObject
	{
		Q_OBJECT
	private:

	public:
		Broadcaster();
		~Broadcaster();

		void addNotifier() {
		}

		void broadcast(int id, const std::string& message) const {
		}
	};

} // namespace SEGUIApp

