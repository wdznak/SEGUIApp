#pragma once

namespace SEGUIApp {

	class TradeStatsI {
	public:
		virtual void trade(const class TradeUpdate&) = 0;
		virtual void printDebug() = 0;
	};

}