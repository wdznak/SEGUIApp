#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <QObject>
#include <QDebug>

#include "DispatchI.h"
#include "RecentTradesModel.h"

namespace SEGUIApp {

	struct Models: public QObject {
		Q_OBJECT
	public:
		std::unique_ptr<RecentTradesModel> recentTrades;

	void onNewTrade(const Trade& data) {
			recentTrades->insertData(data);
		}
	};

	class ModelsManager 
	{
	private:
		std::vector<std::unique_ptr<Models>> modelList_;

	public:
		ModelsManager() = default;
		~ModelsManager() = default;

		Models* createModels() {
			auto models = std::make_unique<Models>();
			models->recentTrades = std::make_unique<RecentTradesModel>();
			modelList_.push_back(std::move(models));
			return modelList_.back().get();
		}

		void removeModels(Models* models) {
			auto& it = std::find_if(modelList_.begin(), modelList_.end(), [&](std::unique_ptr<Models>& models_) {
				return models_.get() == models;
			});

			if (it != modelList_.end()) {
				modelList_.erase(it);
			}
		}
	};

} // namespace SEGUIApp