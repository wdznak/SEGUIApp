#pragma once

#include <memory>

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QTime>

#include "BasicStatistics.h"
#include "BookDepthModel.h"
#include "StatisticsI.h"

namespace SEGUIApp {

	class DataAggregator : public QObject
	{
		Q_OBJECT
	private:
		const BookDepthModel& bookDepth_;
		size_t interval_;
		BookDepthModel::baList_t orderBook_;
		_int64 startTimeStamp_ = 0;
		_int64 endTimeStamp_ = 0;
		std::unique_ptr<StatisticsI> statistics_;

	public:
		// Interval has to be provided in minutes!
		DataAggregator(BookDepthModel& bookDepth, size_t interval)
			: bookDepth_(bookDepth), interval_(interval) 
		{
			statistics_ = std::make_unique<BasicStatistics>();
		};

		/*
		 * Move a timestamp to the beggining of the first valid interval
		 * If the interval = 15 and  timestamp = 2012-10-01 03:24:11
		 * Then first valid interval timestamp = 2012-10-01 03:30:00
		 */
		void init(QDateTime timeStamp) {
			QTime time = timeStamp.time();
			qDebug() << timeStamp;

			if (!time.minute() % interval_ == 0 && !time.second() == 0) {
				if (time.minute() % interval_ == 0 && time.second() > 0) {
					time = time.addSecs(interval_ * 60);
				}
				else {
					int diff = abs(time.minute() - 60) % interval_;
					time = time.addSecs(diff * 60);
				}

				if (time.hour() == 0) {
					timeStamp = timeStamp.addDays(1);
				}
			}

			time.setHMS(time.hour(), time.minute(), 0);
			timeStamp.setTime(time);
			
			startTimeStamp_ = timeStamp.toMSecsSinceEpoch();
			endTimeStamp_ = timeStamp.addSecs(interval_ * 60).toMSecsSinceEpoch();
			qDebug() << QDateTime::fromMSecsSinceEpoch(startTimeStamp_);
			qDebug() << QDateTime::fromMSecsSinceEpoch(endTimeStamp_);
			statistics_->startInterval(startTimeStamp_);
			// init depth in statistics if necessery
		}

		void onDepthUpdated(const DepthUpdate& update) {
			static uint count = 0;
			static _int64 prevU = 0;
			if (update.eventTime < startTimeStamp_) {
				return;
			}
			else if (update.eventTime >= endTimeStamp_) {
				statistics_->endInterval(endTimeStamp_);
				statistics_->startInterval(endTimeStamp_);

				qDebug() << "Depths: " << count;

				startTimeStamp_ = endTimeStamp_;
				endTimeStamp_ += 60000 * interval_; // 60000 = minute in milliseconds
				// Todo: log timestamps to see if there are no holes between
				count = 0;
			}

			if (prevU == update.firstUpdateID) {
				qFatal("Duplicated update!");
			}

			prevU = update.firstUpdateID;
			count++;
		}

		void onTrade(const TradeUpdate& trade) {
			static uint count = 0;
			
			if (trade.eventTime < startTimeStamp_) {
				return;
			}
			else if (trade.eventTime >= endTimeStamp_) {
				statistics_->endInterval(endTimeStamp_);
				statistics_->startInterval(endTimeStamp_);

				startTimeStamp_ = endTimeStamp_;

				qDebug() << "Trades: " << count;
				endTimeStamp_ += 60000 * interval_;
				count = 0;
			}

			statistics_->updateTrade(trade);
			count++;
		}

		void printDebug() {
			statistics_->printDebug();
		}

	private:
	};

} // namespace SEGUIApp

