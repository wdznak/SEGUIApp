#pragma once

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QTime>

#include "BookDepthModel.h"

namespace SEGUIApp {

	class DataAggregator
	{
	private:
		const BookDepthModel& bookDepth_;
		size_t interval_;
		_int64 startTimeStamp_ = 0;
		_int64 endTimeStamp_ = 0;
	public:
		// Interval has to be provided in minutes!
		DataAggregator(BookDepthModel& bookDepth, size_t interval)
			: bookDepth_(bookDepth), interval_(interval) {};

		/*
		 * Move timestamp to the beggining of the first valid interval
		 * If the interval = 15 and  timestamp = 2012-10-01 03:24:11
		 * Then first valid interval timestamp = 2012-10-01 03:30:00
		 */
		void init(QDateTime timeStamp) {
			QTime time = timeStamp.time();
			if (time.minute() % interval_ == 0 && time.second() > 0) {
				time = time.addSecs(interval_ * 60);
			}
			else {
				int diff = (time.minute() - 60) % interval_;
				time = time.addSecs(diff * 60);
			}
			
			if (time.hour() == 0) {
				timeStamp = timeStamp.addDays(1);
			}
			time.setHMS(time.hour(), time.minute(), 0);
			timeStamp.setTime(time);

			startTimeStamp_ = timeStamp.toMSecsSinceEpoch();
			endTimeStamp_ = timeStamp.addSecs(interval_ * 60).toMSecsSinceEpoch();
		}

	};

} // namespace SEGUIApp

