#pragma once

#include <fstream>

#include <QtCore/QDateTime>

#include <rapidjson/document.h>

#include "BookDepthModel.h"

namespace SEGUIApp {

	namespace rj = rapidjson;

	class FileDataParser : public QObject
	{
		Q_OBJECT
	private:
		using baList_t = BookDepthModel::baList_t;

		const static size_t BUFF_SIZE = 131072;
		char buffer[BUFF_SIZE];
		rj::Document document_;
		_int64 from_ = 0;
		bool hasNext_ = false;
		_int64 lastUpdateID_ = 0;
		BookDepthModel& model_;
		_int64 startTime_ = -1;
		bool reachedEnd_ = false;
		std::ifstream stream_;
		_int64 to_ = 0;

	public:
		FileDataParser(BookDepthModel& model)
			: model_(model) {}

		FileDataParser(BookDepthModel& model, _int64 from, _int64 to)
			: model_(model), from_(from), to_(to) {}

		_int64 getStartTime() const {
			return startTime_;
		}

		bool hasNext() {
			return hasNext_;
		}

		bool initBook(const QString& fileName, _int64 from = 0, _int64 to = 0) {
			from_ = from;
			to_ = to;
			qDebug() << "FROM: " << QDateTime::fromMSecsSinceEpoch(from);
			qDebug() << "TO: " << QDateTime::fromMSecsSinceEpoch(to);
			stream_.open(fileName.toStdString());
			size_t updateId;

			if (!stream_.is_open()) {
				hasNext_ = false;
				return false;
			}
			
			if (hasNext_ = static_cast<bool>(stream_.getline(&buffer[0], BUFF_SIZE, '\r\n'))) {
				document_.Parse(&buffer[0]);

				if (!document_.IsObject()) {
					qFatal("Validation failed!");
				}

				if (document_.HasMember("lastUpdateId")) {
					updateId = document_["lastUpdateId"].GetInt();
					const rj::Value& bids = document_["bids"];
					const rj::Value& asks = document_["asks"];
					baList_t bidsList;
					baList_t asksList;

					std::for_each(bids.Begin(), bids.End(), [&bidsList](const rj::Value& bid) {
						bidsList.emplace_back(std::initializer_list<std::string>{bid[0].GetString(), bid[1].GetString()});
						});

					std::for_each(asks.Begin(), asks.End(), [&asksList](const rj::Value& ask) {
						asksList.emplace_back(std::initializer_list<std::string>{ask[0].GetString(), ask[1].GetString()});
						});

					model_.fillBook(updateId, bidsList, asksList);

					while (!stream_.getline(&buffer[0], BUFF_SIZE, '\r\n').eof()) {
						hasNext_ = true;
						document_.Parse(&buffer[0]);
						const rj::Value& data = document_["data"];

						if (document_.HasMember("stream") && data["e"] == "depthUpdate") {
							if (data["u"].GetInt() <= updateId) {
								continue;
							}
							else if (data["U"].GetInt() <= updateId + 1 &&
								data["u"].GetInt() >= updateId + 1)
							{
								startTime_ = data["E"].GetInt64();

								DepthUpdate update;
								update.eventTime = data["E"].GetInt64();
								update.firstUpdateID = data["U"].GetInt64();
								update.lastUpdateID = data["u"].GetInt64();
								const rj::Value& bids = data["b"];
								const rj::Value& asks = data["a"];

								lastUpdateID_ = update.lastUpdateID;

								std::for_each(bids.Begin(), bids.End(), [&](const rj::Value& bid) {
									std::string price = bid[0].GetString();
									std::string size = bid[1].GetString();
									update.bids.push_back(BidAsk{ price.data(), size.data() });

									model_.updateBidAsk(true, price, size, update.firstUpdateID, update.lastUpdateID);
									update.bids.emplace_back(price.data(), size.data());
									});

								std::for_each(asks.Begin(), asks.End(), [&](const rj::Value& ask) {
									std::string price = ask[0].GetString();
									std::string size = ask[1].GetString();

									model_.updateBidAsk(false, price, size, update.firstUpdateID, update.lastUpdateID);
									update.asks.emplace_back(price.data(), size.data());
									});

								return true;
							}
							else {
								qFatal("Book depth not valid!");
							}
						}
					}
				}
			}

			return false;
		}

		bool loadFile(const QString& fileName) {
			if (stream_.is_open()) {
				stream_.close();
			}

			stream_.open(fileName.toStdString());

			if (!stream_.is_open()) {
				hasNext_ = false;
				return false;
			}

			return hasNext_ = true;
		}

		void next() {
			if (!stream_.getline(&buffer[0], BUFF_SIZE, '\r\n').eof()) {
				hasNext_ = true;
				document_.Parse(&buffer[0]);

				if (document_.HasMember("stream")) {
					const rj::Value& data = document_["data"];

					if (std::string(data["e"].GetString()) == "depthUpdate") {
						hasNext_ = updateDepth(data);
					}
					else if (std::string(data["e"].GetString()) == "trade") {
						hasNext_ = updateTrade(data);
					}
				}
			}
			else {
				hasNext_ = false;
			}
		}

		bool reachedEnd() {
			return reachedEnd_;
		}

	private:
		bool updateDepth(const rj::Value& data) {
			DepthUpdate update;
			update.eventTime      = data["E"].GetInt64();
			update.firstUpdateID  = data["U"].GetInt64();
			update.lastUpdateID   = data["u"].GetInt64();
			const rj::Value& bids = data["b"];
			const rj::Value& asks = data["a"];

			if (lastUpdateID_ + 1 != update.firstUpdateID) {
				qFatal("Depth update ID gap!");
			}

			lastUpdateID_ = update.lastUpdateID;

			std::for_each(bids.Begin(), bids.End(), [&](const rj::Value& bid) {
				std::string price = bid[0].GetString();
				std::string size  = bid[1].GetString();
				update.bids.push_back(BidAsk{ price.data(), size.data() });

				model_.updateBidAsk(true, price, size, update.firstUpdateID, update.lastUpdateID);
				update.bids.emplace_back(price.data(), size.data());
			});

			std::for_each(asks.Begin(), asks.End(), [&](const rj::Value& ask) {
				std::string price = ask[0].GetString();
				std::string size = ask[1].GetString();

				model_.updateBidAsk(false, price, size, update.firstUpdateID, update.lastUpdateID);
				update.asks.emplace_back(price.data(), size.data());
			});
			
			if (from_ != 0 && from_ > update.eventTime && to_ >= update.eventTime) {
				return true;
			}
			else if (to_ != 0 && to_ < update.eventTime) {
				reachedEnd_ = true;
				return false;
			}

			emit depthUpdated(update);

			return true;
		}

		bool updateTrade(const rj::Value& data) {
			TradeUpdate update;
			update.eventTime = data["E"].GetInt64();
			update.tradeTime = data["T"].GetInt64();
			update.buyerId   = data["b"].GetInt64();
			update.sellerId  = data["a"].GetInt64();
			update.price     = data["p"].GetString();
			update.quantity  = data["q"].GetString();
			update.isBuyerMarketMaker = data["m"].GetBool();
			
			if (from_ != 0 && from_ > update.eventTime && to_ >= update.eventTime) {
				return true;
			}
			else if (to_ != 0 && to_ < update.eventTime) {
				reachedEnd_ = true;
				return false;
			}

			emit trade(update);

			return true;
		}

	signals:
		void depthUpdated(const DepthUpdate&);
		void trade(const TradeUpdate&);
	};

} // namespace SEGUIApp