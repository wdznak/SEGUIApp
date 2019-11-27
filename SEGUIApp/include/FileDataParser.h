#pragma once

#include <rapidjson/document.h>

#include "BookDepthModel.h"

namespace SEGUIApp {

	namespace rj = rapidjson;

	class FileDataParser
	{
	private:
		using baList_t = BookDepthModel::baList_t;

		BookDepthModel& model_;
		const static size_t BUFF_SIZE = 131072;
		char buffer[BUFF_SIZE];
		rj::Document document_;

	public:
		FileDataParser(BookDepthModel& model)
			: model_(model) 
		{
			std::ifstream stream("C:\\bin_btc-tusd.txt");

			if (!stream.is_open()) {
				qFatal("Can't open a file!");
			}
			else {
				qDebug() << "File is open!";
			}

			while (stream.getline(&buffer[0], BUFF_SIZE, '\r\n')) {

				document_.Parse(&buffer[0]);

				if (document_.IsObject()) {
					//qDebug() << "Validation succeded!";
				}
				else {
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
						const rj::Value& bids = data["b"];
						const rj::Value& asks = data["a"];

						std::for_each(bids.Begin(), bids.End(), [&](const rj::Value& bid) {
							std::string price = bid[0].GetString();
							std::string size = bid[1].GetString();

							model_.updateBids(price, size);
						});

						std::for_each(asks.Begin(), asks.End(), [&](const rj::Value& ask) {
							std::string price = ask[0].GetString();
							std::string size = ask[1].GetString();

							model_.updateAsks(price, size);
						});
					}
				}
			}
		}

	};

} // namespace SEGUIApp