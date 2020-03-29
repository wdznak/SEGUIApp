#pragma once

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
			qDebug() << "Intervals: " << intervals_.size();
			qDebug() << "Open: " << startIntervals_ << " Close: " << endIntervals_;
			for (auto e : intervals_) {
				qDebug() << "From: " << QDateTime::fromMSecsSinceEpoch(e.startTime);
				qDebug() << "To:" << QDateTime::fromMSecsSinceEpoch(e.endTime);
				e.tradeStats.printDebug();
			}
		}
	};

} // namespace SEGUIApp

