#include "RecentTradesModel.h"

namespace SEGUIApp {

	RecentTradesModel::RecentTradesModel()
	{
	}


	RecentTradesModel::~RecentTradesModel()
	{
	}

	// Data is returned to the view in reverse order so the newest 
	// entries are on the top of the view.
	QVariant RecentTradesModel::data(const QModelIndex & index, int role) const {
		if (role == Qt::DisplayRole) {
			if (index.row() > currentSize) return QVariant{};
			int tempIdx = (endIndex_ - 1 - index.row() + currentSize) % currentSize;

			switch (index.column()) {
				case 0: return data_[tempIdx].price;
				case 1: return data_[tempIdx].size;
				case 2: return data_[tempIdx].time;
				case 3: return data_[tempIdx].isBuy ? "B" : "S";
				default: return QVariant{};
			}
		}

		return QVariant{};
	}

	QVariant RecentTradesModel::headerData(int section, Qt::Orientation orientation, int role) const
	{
		if (role != Qt::DisplayRole)
			return QVariant();

		if (orientation == Qt::Horizontal) {
			switch (section) {
				case 0: return tr("Price");
				case 1: return tr("Size");
				case 2: return tr("Time");
				case 3: return tr("Buy\\Sell");
				default: return QVariant();
			}
		}

		return QVariant();
	}

} // namespace SEGUIApp
