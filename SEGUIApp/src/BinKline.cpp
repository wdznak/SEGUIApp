#include "BinKline.h"

namespace SEGUIApp {

	BinKline::BinKline(const QString& symbol, INTERVAL interval, long startTime, long endTime, int limit)
		: symbol_(symbol), interval_(interval), startTime_(startTime), endTime_(endTime), limit_(limit)
	{
		fileName_ = QString(symbol.data()) + intervalList_.at(interval) + ".txt";
		QFile file(fileName_);

		if (!file.exists(fileName_)) {

		}

		if (!file.open(QIODevice::ReadWrite)) {
			// create file and load data from binance
		}
	}

	bool BinKline::loadInterval(INTERVAL interval) {
		if (!klineList_.count(interval)) {
			QString fileName = fileName_.arg(symbol_, intervalList_.at(interval));
			if (!QFile::exists(fileName_)) {
				QFile file(fileName_);
				file.open(QIODevice::ReadWrite);
				file.close();
				// updateFile(QFile& );
				// load data to vector();
			}
		}
	}

	QString BinKline::getKline(long startTime, long endTime, int limit) {
		if (!endTime) {
			endTime = QDateTime::currentMSecsSinceEpoch();
		}
		QString url = binKlineAddress_.arg(symbol_,
			intervalList_.at(interval_),
			QString::number(startTime),
			QString::number(endTime),
			QString::number(limit_));

		//auto response = cpr::Get(cpr::Url(""), cpr::VerifySsl(false));
	}

	bool BinKline::updateFile(const QString& fileName) {
		csv::Reader csv;
		csv.read(fileName.toStdString());
		// This may fail with very large files. Use p-ranav github to see streaming version.
		auto rows = csv.rows();

		csv::Writer csvFile(fileName.toStdString());
		csvFile.configure_dialect()
			.delimiter(", ")
			.column_names("Open Time", "Open Price", "High Price", "Low Price", "Close Price", "Volume", "CloseTime", "Trades Number");

		if (!rows.size()) {
			//create column names for csv
		}
		else {
			//get newest timestamp and strat geting data till current time
		}
	}

} // namespace SEGUIApp