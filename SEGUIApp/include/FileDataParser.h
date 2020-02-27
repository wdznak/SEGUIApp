#pragma once

#include <QtCore/QDateTime>

#include <rapidjson/document.h>

#include "BookDepthModel.h"

namespace SEGUIApp {

	namespace rj = rapidjson;

	class FileDataParser
	{
	private:
		using baList_t = BookDepthModel::baList_t;

		const static size_t BUFF_SIZE = 131072;
		char buffer[BUFF_SIZE];
		rj::Document document_;
		bool hasNext_ = false;
		BookDepthModel& model_;
		std::ifstream stream_;

	public:
		FileDataParser(BookDepthModel& model)
			: model_(model) 
		{
		}

		bool hasNext() {
			return hasNext_;
		}

		void next() {
			if (hasNext_ = static_cast<bool>(stream_.getline(&buffer[0], BUFF_SIZE, '\r\n'))) {
				document_.Parse(&buffer[0]);

				if (!document_.IsObject()) {
					qFatal("Validation failed!");
				}

				if (document_.HasMember("lastUpdateId")) {
					size_t updateId_ = document_["lastUpdateId"].GetInt();
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

					model_.fillBook(updateId_, bidsList, asksList);
				}
				else if (document_.HasMember("stream")) {
					const rj::Value& data = document_["data"];

					if (std::string(data["e"].GetString()) == "depthUpdate") {
						qDebug() << QDateTime::fromMSecsSinceEpoch(data["E"].GetUint64());
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

		bool openFile(const std::string& file) {
			stream_.open(file);

			if (stream_.is_open()) {
				hasNext_ = true;
				return true;
			}

			return false;
		}

	};

} // namespace SEGUIApp