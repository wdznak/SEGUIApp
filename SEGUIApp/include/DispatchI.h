#pragma once

#include <string>

#include <QtCore/QMetaType>
#include <QtCore/QObject>

namespace SEGUIApp {

	struct Trade {
		QString price;
		QString size;
		size_t time = 0;
		QString isBuy;
	};

	struct Depth {
		Depth(QString p, QString s) : priceLvl(std::move(p)), size(std::move(s)) {}
		QString priceLvl;
		QString size;
	};

	struct DepthUpdate {
		std::vector<Depth> asks;
		std::vector<Depth> bids;
		size_t firstUpdateId = 0;
		size_t finalUpdateId = 0;
		size_t time = 0;
	};

	class DispatchI: public QObject {
		Q_OBJECT
	public:
		virtual ~DispatchI() = default;
		virtual void dispatch(int connectionId, const std::string& message) = 0;

	signals:
		void depthUpdated(const DepthUpdate&);
		void tradeCreated(const Trade&);
	};

} // namespace SEGUIApp

Q_DECLARE_METATYPE(SEGUIApp::Trade)