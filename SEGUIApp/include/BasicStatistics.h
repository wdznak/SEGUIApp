#pragma once

#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <vector>

#include <QtCore/QDateTime>
#include <QtCore/QDebug>

#include "ANamespace.h"
#include "StatisticsI.h"
#include "BasicTradeStats.h"

namespace SEGUIApp {

	class BasicStatistics : public StatisticsI
	{
	private:
		struct comp {
			bool operator()(const std::pair<_int64, double>& a, const std::pair<_int64, double>& b) const {
				return a.second > b.second;
			}
		};

		struct Interval {
			_int64 startTime = 0;
			_int64 endTime = 0;
			BasicTradeStats tradeStats;
		};

		std::vector<Interval> intervals_;
		size_t startIntervals_ = 0;
		size_t endIntervals_ = 0;
		BasicTradeStats* tradeStats_;

	public:
		BasicStatistics() {}

		void updateDepth() override {}
		void updateTrade(const TradeUpdate& trade) override {
			tradeStats_->trade(trade);
		}

		void endInterval(_int64 endTimeInterval) override {
			if (startIntervals_ -1 != endIntervals_) {
				qFatal("End interval desynchronization in BasicStatics!");
			}

			intervals_[intervals_.size() - 1].endTime = endTimeInterval;
			endIntervals_++;
		}

		void startInterval(_int64 startTimeInterval) override {
			if (startIntervals_ != endIntervals_) {
				qFatal("Start interval desynchronization in BasicStatics!");
			}

			Interval interval;
			interval.startTime = startTimeInterval;
			intervals_.emplace_back(interval);

			if (intervals_.size() == 1) {
				tradeStats_ = &intervals_[0].tradeStats;
			}
			else {
				size_t size = intervals_.size();
				tradeStats_ = &intervals_[size - 1].tradeStats;
				//intervals_[size - 2].tradeStats.printDebug();
			}

			startIntervals_++;
		}

		void printDebug() override {
			std::unordered_map<_int64, double> value;

			for (auto e : intervals_) {
				qDebug() << "From: " << QDateTime::fromMSecsSinceEpoch(e.startTime);
				if (e.endTime == 0) {
					qDebug() << "To:" << QDateTime::fromMSecsSinceEpoch(tradeStats_->lastUpdateTimestamp_);
				}
				else {
					qDebug() << "To:" << QDateTime::fromMSecsSinceEpoch(e.endTime);
				}
				e.tradeStats.printDebug();
				for (auto f : e.tradeStats.biggestFish_) {
					value[f.first] += f.second.totalAmount;
				}
			}
			qDebug() << "\n";

			std::vector<std::pair<_int64, double>> elems(value.begin(), value.end());
			std::sort(elems.begin(), elems.end(), comp());

			int users = 10;
			double totalBuy = 0;
			double totalSell = 0;

			int totalUsers, totalBuyers, totalSellers;
			const auto dcomp = [](const std::pair<_int64, double>& a, double b) -> bool {
				return a.second > b;
			};

			auto mid = std::lower_bound(elems.begin(), elems.end(), 0, dcomp);

			totalUsers = elems.size();
			totalBuyers = std::distance(elems.begin(), mid);
			totalSellers = std::distance(mid, elems.end());

			qDebug() << "Total buyers--------------- " << totalBuyers << " --------------------";
			for (int i = 0; i <= totalBuyers; ++i) {
				totalBuy += elems[i].second;
			}
			qDebug() << totalBuy << "\n";
			
			qDebug() << "Total sellers----------------- " << totalSellers << " ------------------";
			for (int i = elems.size() - 1; i > elems.size() - totalBuyers; --i) {
				totalSell += elems[i].second;
			}
			qDebug() << totalSell << "\n\n";
			double perDiff = ((totalBuyers - totalSellers) / ((totalBuyers + totalSellers) / 2.0f)) * 100;
			qDebug() << "user % diff buy/sell: " << perDiff << "\n";

			const double perTreshold = 0.05f;
			int valTreshold = totalBuyers * perTreshold;
			double currAmountB = 0;
			int topBuyUCount = 0;
			int topSellUCount = 0;

			for (int i = 0; i <= valTreshold; ++i) {
				currAmountB += elems[i].second;
			}

			qDebug() << "Top " << valTreshold << " Buyers ------------------------------";
			qDebug() << "Bought:         " << currAmountB;
			qDebug() << "The rest bought:" << (totalBuy - currAmountB) << "\n";

			valTreshold = totalSellers * perTreshold;
			double currAmountS = 0;
			auto rmid = elems.rbegin();

			std::advance(rmid, totalSellers);

			for (int i = elems.size() - 1; i > elems.size() - 1 - valTreshold; --i) {
				currAmountS += std::abs(elems[i].second);
			}

			qDebug() << "Top " << valTreshold << " Sellers ------------------------------";
			qDebug() << "Sold:         " << currAmountS;
			qDebug() << "The rest sold:" << (totalSell + currAmountS) << "\n";

			qDebug() << "Whales diff: " << currAmountB - currAmountS;
			qDebug() << "Fish diff:   " << (totalBuy - currAmountB) - std::abs(totalSell + currAmountS);

			qDebug() << "Intervals: " << intervals_.size();
			qDebug() << "Open: " << startIntervals_ << " Close: " << endIntervals_;
			qDebug() << "Total Buy:  " << totalBuy;
			qDebug() << "Total Sell: " << totalSell;
		}
	};

} // namespace SEGUIApp

