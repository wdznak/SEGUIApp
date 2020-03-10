#pragma once

#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>

#include <QtCore/QDateTime>
#include <QtCore/QFile>

#include <csv/reader.hpp>
#include <csv/writer.hpp>
#include <cpr/cpr.h>

namespace SEGUIApp {

	struct Kline {
		long openTime;
		long openPrice;
		long highPrice;
		long lowPrice;
		long closePrice;
		long volume;
		long closeTime;
		int tradesNumber;
	};

	struct Range {
		QDateTime startTime;
		QDateTime endTime;
	};

	class BinKline
	{
	public:
		enum class INTERVAL {
			I1M,
			I5M,
			I15M,
			I30M,
			I1H,
			I1D,
			I1W,
			I1MO
		};

	private:
		static const long binMinDate = 1502928000000;

		QString binKlineAddress_{ "https://www.binance.com/api/v3/klines?symbol=%1&interval=%2&startTime=%3&endTime=%4limit=%5" };
		QString fileName_{"%1_%2_Klines.csv"};
		long endTime_;
		INTERVAL interval_ = INTERVAL::I30M;
		std::unordered_map<INTERVAL, QString> intervalList_{
			{INTERVAL::I1M, "1m"}, {INTERVAL::I5M, "5m"}, {INTERVAL::I15M, "15m"}, {INTERVAL::I30M, "30m"},
			{INTERVAL::I1H, "1h"}, {INTERVAL::I1D, "1d"}, {INTERVAL::I1W, "1w"}, {INTERVAL::I1MO, "1mo"}
		};
		std::unordered_map<INTERVAL, std::vector<Kline>> klineList_;
		int limit_ = 1000;
		long startTime_;
		QString symbol_;

	public:
		BinKline(const QString& symbol, INTERVAL interval, long startTime, long endTime, int limit);
		bool loadInterval(INTERVAL interval);

	private:
		QString getKline(long startTime, long endTime = 0, int limit = 1000);
		bool updateFile(const QString& fileName);
	};

} // namespace SEGUIApp

