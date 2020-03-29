#pragma once

namespace SEGUIApp {

	class StatisticsI {
	public:
		virtual void updateDepth() = 0;
		virtual void updateTrade(const struct TradeUpdate& trade) = 0;
		virtual void endInterval(_int64 endTimeInterval) {};
		virtual void startInterval(_int64 startTimeInterval) {};
		virtual void printDebug() {};
	};

} // namespace SEGUIApp
