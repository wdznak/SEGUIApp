#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>

#include <QString>
#include <QtCore/QDebug>

#include "ANamespace.h"
#include "TradeStatsI.h"

namespace SEGUIApp {

	struct UserData {
		double totalAmount = 0;
		double buyAmount = 0;
		double sellAmount = 0;
		size_t totalTrans = 0;
		size_t buyTrans = 0;
		size_t sellTrans = 0;
	};

	struct BasicTradeStruct {
		std::unordered_map<_int64, UserData> users;
		size_t priceDown = 0;
		size_t priceUp = 0;
		size_t totalTransactions = 0;
		double volume = 0;
	};
	
	struct comp {
		bool operator()(const std::pair<_int64, UserData>& a, const std::pair<_int64, UserData>& b) const {
			return a.second.totalAmount > b.second.totalAmount;
		}
	};

	class BasicTradeStats : public TradeStatsI {
	public:
		BasicTradeStruct data;
		_int64 lastUpdateTimestamp_;
	private:
		bool isUp_ = true;
		double refPrice_ = 0;
	public:
		BasicTradeStats() = default;
		BasicTradeStats(QString refPrice)
		: refPrice_(refPrice.toDouble()) {}

		void trade(const TradeUpdate& trade) override {
			lastUpdateTimestamp_ = trade.eventTime;
			double price = trade.price.toDouble();
			double quantity = trade.quantity.toDouble();

			data.totalTransactions++;
			data.volume += quantity;

			data.users[trade.buyerId].totalAmount += quantity;
			data.users[trade.buyerId].totalTrans++;
			data.users[trade.buyerId].buyTrans++;
			data.users[trade.buyerId].buyAmount += quantity;

			data.users[trade.sellerId].totalAmount -= quantity;
			data.users[trade.sellerId].totalTrans++;
			data.users[trade.sellerId].sellTrans++;
			data.users[trade.sellerId].sellAmount += quantity;

			if (refPrice_ == price) {
				if (isUp_) {
					data.priceUp++;
				}
				else {
					data.priceDown++;
				}
				return;
			}
			else if (refPrice_ < price) {
				isUp_ = true;
				data.priceUp++;
			}
			else {
				isUp_ = false;
				data.priceDown++;
			}

			refPrice_ = price;
		}

		void printDebug() override {
			qDebug() << "Transactions: " << data.totalTransactions << " | Volume: " << data.volume << "\n"
				<< "Price went up: " << data.priceUp << " | Price went down: " << data.priceDown;

			/*std::vector<std::pair<_int64, UserData>> elems(biggestFish_.begin(), biggestFish_.end());
			std::sort(elems.begin(), elems.end(), comp());

			int c = 0;
			int users = 5;
			double buySum = 0;
			double sellSum = 0;

			for (auto it = elems.begin(); it != elems.end() && c < users; ++it, ++c) {
				buySum += it->second.totalAmount;
				qDebug() << "ID: " << it->first << "\n"
					<< " | Total transactions:     " << it->second.totalTrans << "\n"
					<< " | Total amount in pocket: " << it->second.totalAmount << "\n"
					<< " | Total buys:             " << it->second.buyTrans << "\n"
					<< " | Buy amount:             " << it->second.buyAmount << "\n"
					<< " | Total sells:            " << it->second.sellTrans << "\n"
					<< " | Sell amount:            " << it->second.sellAmount;
			}

			c = 0;

			for (auto it = elems.rbegin(); it != elems.rend() && c < users; ++it, ++c) {
				sellSum += it->second.totalAmount;
				qDebug() << "ID: " << it->first << "\n"
					<< " | Total transactions:     " << it->second.totalTrans << "\n"
					<< " | Total amount in pocket: " << it->second.totalAmount << "\n"
					<< " | Total buys:             " << it->second.buyTrans << "\n"
					<< " | Buy amount:             " << it->second.buyAmount << "\n"
					<< " | Total sells:            " << it->second.sellTrans << "\n"
					<< " | Sell amount:            " << it->second.sellAmount;
			}

			qDebug() << "Total bought: " << buySum << "\n"
				<< "Total sold: " << sellSum;*/
		}
	};

} // namespace SEGUIApp