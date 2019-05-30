#pragma once

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include <QObject>

#include "BinMessageDispatch.h"
#include "DispatchI.h"
#include "ANamespace.h"

namespace SEGUIApp {

	class MessageDispatch {
	private:
		std::unordered_map<std::string, Exchange> exchangeDictionary{ {"binance", Exchange::BINANCE}, {"bitmex", Exchange::BITMEX} };

	public:
		std::shared_ptr<DispatchI> createDispatch(std::string exchangeName) {
			std::transform(exchangeName.begin(), exchangeName.end(), exchangeName.begin(), ::tolower);
			Exchange name = exchangeDictionary.at(exchangeName);
			
			switch (name) {
			case Exchange::BINANCE: return std::make_shared<BinMessageDispatch>();
			default: return nullptr;
			}
		}
	};

} // namespace SEGUIApp