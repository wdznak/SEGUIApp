#pragma once

#include <string>
#include <vector>

#include <QtCore/QObject>
#include <QtCore/QString>

#include <rapidjson/document.h>

#include "DispatchI.h"

namespace SEGUIApp {

	namespace rj = rapidjson;

	template<typename T>
	const char* GetString(T&& d, const char* v) {
		auto it = d.FindMember(v);
		if (it != d.MemberEnd() && it->value.IsString()) {
			return it->value.GetString();
		}
		else {
			return "";
		}
	}

	template<typename T>
	size_t GetUint64(T&& d, const char* v) {
		auto it = d.FindMember(v);
		if (it != d.MemberEnd() && it->value.IsNumber()) {
			return it->value.GetUint64();
		}
		else {
			return 0;
		}
	}

	template<typename T>
	char GetBool(T&& d, const char* v) {
		auto it = d.FindMember(v);
		if (it != d.MemberEnd() && it->value.IsBool()) {
			return it->value.GetBool() ? 'B' : 'S';
		}
		else {
			return 'E';
		}
	}

	class BinMessageDispatch: public DispatchI
	{
		Q_OBJECT

	public:
		BinMessageDispatch() = default;
		~BinMessageDispatch() = default;

		void dispatch(int connectionId, const std::string& message) {
			rj::Document document_;
			document_.Parse(message.data());

			if (!document_.IsObject()) {
				return;
				// TODO: Handle error.
			}

			if (document_.HasMember("stream") && document_.HasMember("data")) {
				if (document_["data"].IsObject()) {
					const auto data = document_["data"].GetObject();
					createMessage(data);
				}
			}
			else if (document_.HasMember("e")) {
				createMessage(document_);
			}
			else if (document_.HasMember("lastUpdateId")) {

			}
			else {
				// TODO: Handle error.
			}
		}

	private:
		template<typename T>
		void createMessage(const T& data) {
			std::string streamName = GetString(data, "e");

			if ("trade" == streamName) {
				Trade tradeData;
				tradeData.price = GetString(data, "p");
				tradeData.size  = GetString(data, "q");
				tradeData.time  = GetUint64(data, "T");
				tradeData.isBuy = GetBool(data, "m");
				emit tradeCreated(tradeData);
			} 
			else if ("depthUpdate" == streamName) {
				DepthUpdate depthData;
				depthData.firstUpdateId = GetUint64(data, "U");
				depthData.finalUpdateId = GetUint64(data, "u");
				depthData.time		    = GetUint64(data, "E");
				depthData.asks			= getDepthArray(data, "a");
				depthData.bids			= getDepthArray(data, "b");
				emit depthUpdated(depthData);
			}
		}

		template <typename T>
		std::vector<Depth> getDepthArray(T&& data, const char* value) {
			std::vector<Depth> result;
			auto it = data.FindMember(value);
			
			if (it != data.MemberEnd() && it->value.IsArray() && !it->value.Empty()) {
				result.reserve(it->value.Capacity());

				for (auto& v : it->value.GetArray()) {
					result.emplace_back(v[0].GetString(), v[1].GetString());
				}
			}

			return result;
		}
	};
	
} // namespace SEGUIApp