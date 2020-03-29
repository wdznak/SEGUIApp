#pragma once

#include <map>
#include <vector>

#include <QString>

#include <QString>

namespace SEGUIApp {

	enum class Message {
		M_ERROR,
		M_WARNING,
		M_INFO
	};

	enum class Exchange {
		BINANCE,
		BITMEX,
		NONE
	};

	enum class Event {
		TRADE,
		DEPTH
	};

	static const std::map<Message, QString> messageIco = {
		{Message::M_ERROR, ":/SEGUIApp/Resources/Icons/1x/error_round.png"},
		{Message::M_WARNING, ":/SEGUIApp/Resources/Icons/1x/warning_round.png"},
		{Message::M_INFO, ":/SEGUIApp/Resources/Icons/1x/info_round.png"}
	};

	struct BidAsk {
		BidAsk(QString p, QString s) 
			: price(std::move(p)), quantity(std::move(s)) {}
		QString price;
		QString quantity;
	};

	struct DepthUpdate {
		_int64 eventTime = 0;
		_int64 firstUpdateID = 0;
		_int64 lastUpdateID = 0;
		std::vector<BidAsk> bids;
		std::vector<BidAsk> asks;
	};

	struct TradeUpdate {
		_int64 eventTime = 0;
		_int64 tradeTime = 0;
		_int64 buyerId = 0;
		_int64 sellerId = 0;
		QString price = 0;
		QString quantity = 0;
		bool isBuyerMarketMaker = false;
	};

}
