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
		bool hasNext_ = false;
		BookDepthModel& model_;
		_int64 startTime_ = -1;
		std::ifstream stream_;

	public:
		FileDataParser(BookDepthModel& model)
			: model_(model) 
		{
		}

		QDateTime getStartTime() const {
			return QDateTime::fromMSecsSinceEpoch(startTime_);
		}

		bool hasNext() {
			return hasNext_;
		}

		bool initBook(const QString& fileName) {
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
				}
			}

			while (hasNext_ = static_cast<bool>(stream_.getline(&buffer[0], BUFF_SIZE, '\r\n'))) {
				document_.Parse(&buffer[0]);

				if (document_.HasMember("stream") && document_["data"]["e"] == "depthUpdate") {
					if (document_["data"]["u"].GetInt() <= updateId) {
						continue;
					}
					else if (document_["data"]["U"].GetInt() <= updateId + 1 &&
							 document_["data"]["u"].GetInt() >= updateId + 1) {
						startTime_ = document_["data"]["E"].GetInt64();
						return true;
					}
					else {
						qFatal("Book depth not valid!");
					}
				}
			}

			return false;
		}

		void next() {
			if (hasNext_ = static_cast<bool>(stream_.getline(&buffer[0], BUFF_SIZE, '\r\n'))) {
				document_.Parse(&buffer[0]);

				if (document_.HasMember("stream")) {
					const rj::Value& data = document_["data"];

					if (std::string(data["e"].GetString()) == "depthUpdate") {
						const rj::Value& bids = data["b"];
						const rj::Value& asks = data["a"];

						std::for_each(bids.Begin(), bids.End(), [&](const rj::Value& bid) {
							std::string price = bid[0].GetString();
							std::string size = bid[1].GetString();

							model_.updateBidAsk(true, price, size);
						});

						std::for_each(asks.Begin(), asks.End(), [&](const rj::Value& ask) {
							std::string price = ask[0].GetString();
							std::string size = ask[1].GetString();

							model_.updateBidAsk(false, price, size);
						});
					}
				}
			}
		}

	signals:
		void depthUpdated();
		void trade();
	};

} // namespace SEGUIApp