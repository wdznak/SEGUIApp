#pragma once

#include <string>

#include <QtCore/QMetaType>
#include <QtCore/QObject>

#include "ANamespace.h"

namespace SEGUIApp {

	struct Trade {
		QString price;
		QString size;
		size_t time = 0;
		QString isBuy;
	};

	class DispatchI: public QObject {
		Q_OBJECT
	private:
		Exchange exchange = Exchange::NONE;
	public:

		virtual ~DispatchI() = default;
		virtual void dispatch(int connectionId, const std::string& message) = 0;
		virtual Exchange getExchange() {
			return exchange;
		};

	signals:
		void depthUpdated(const DepthUpdate&);
		void tradeCreated(const Trade&);
	};

} // namespace SEGUIApp

Q_DECLARE_METATYPE(SEGUIApp::Trade)