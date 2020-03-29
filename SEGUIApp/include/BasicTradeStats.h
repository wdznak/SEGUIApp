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
	
	struct comp {
		bool operator()(const std::pair<_int64, UserData>& a, const std::pair<_int64, UserData>& b) const {
			return a.second.totalAmount < b.second.totalAmount;
		}
	};

	class BasicTradeStats : public TradeStatsI {
	private:
		bool isUp_ = true;
		double refPrice_ = 0;
		size_t priceDown_ = 0;
		size_t priceUp_ = 0;
		size_t totalTransactions_ = 0;
		double volume_ = 0;
		std::unordered_map<_int64, UserData> biggestFish_;
	public:
		BasicTradeStats() = default;
		BasicTradeStats(QString refPrice)
		: refPrice_(refPrice.toDouble()) {}

		void trade(const TradeUpdate& trade) override {
			double price = trade.price.toDouble();
			double quantity = trade.quantity.toDouble();

			totalTransactions_++;
			volume_ += quantity;

			biggestFish_[trade.buyerId].totalAmount += quantity;
			biggestFish_[trade.buyerId].totalTrans++;
			biggestFish_[trade.buyerId].buyTrans++;
			biggestFish_[trade.buyerId].buyAmount += quantity;

			biggestFish_[trade.sellerId].totalAmount -= quantity;
			biggestFish_[trade.sellerId].totalTrans++;
			biggestFish_[trade.sellerId].sellTrans++;
			biggestFish_[trade.sellerId].sellAmount += quantity;

			if (refPrice_ == price) {
				if (isUp_) {
					priceUp_++;
				}
				else {
					priceDown_++;
				}
				return;
			}
			else if (refPrice_ < price) {
				isUp_ = true;
				priceUp_++;
			}
			else {
				isUp_ = false;
				priceDown_++;
			}

			refPrice_ = price;
		}

		void printDebug() override {
			qDebug() << "Transactions: " << totalTransactions_ << " | Volume: " << volume_;

			std::vector<std::pair<_int64, UserData>> elems(biggestFish_.begin(), biggestFish_.end());
			std::sort(elems.begin(), elems.end(), comp());

			int c = 0;

			for (auto it = elems.begin(); it != elems.end() && c < 5; ++it, ++c) {
				qDebug() << "ID: " << it->first << "\n"
					<< " | TOTAL: " << it->second.totalTrans << "\n"
					<< " | SIZE:  " << it->second.totalAmount << "\n"
					<< " | TOTAL: " << it->second.buyTrans << "\n"
					<< " | BUY:   " << it->second.buyAmount << "\n"
					<< " | TOTAL: " << it->second.sellTrans << "\n"
					<< " | SELL:  " << it->second.sellAmount;
			}

			c = 0;

			for (auto it = elems.rbegin(); it != elems.rend() && c < 5; ++it, ++c) {
				qDebug() << "ID: " << it->first << "\n"
					<< " | TOTAL: " << it->second.totalTrans << "\n"
					<< " | SIZE:  " << it->second.totalAmount << "\n"
					<< " | TOTAL: " << it->second.buyTrans << "\n"
					<< " | BUY:   " << it->second.buyAmount << "\n"
					<< " | TOTAL: " << it->second.sellTrans << "\n"
					<< " | SELL:  " << it->second.sellAmount;
			}
		}
	};

} // namespace SEGUIApp