#pragma once
#include <array>

#include <QtCore/QAbstractTableModel>

#include "BinMessageDispatch.h"

namespace SEGUIApp {
	
	// Model is in the form of ring buffer to avoid memory bloat.
	// Maximum size is specified by "maxArraySize".
	class RecentTradesModel : public QAbstractTableModel
	{
	private:
		size_t currentSize = 0;
		size_t endIndex_ = 0;
		static const size_t maxArraySize_ = 16;
		std::array<Trade, maxArraySize_> data_;
	public:
		RecentTradesModel();
		~RecentTradesModel();

		int columnCount(const QModelIndex &parent = QModelIndex()) const override {
			Q_UNUSED(parent);
			return 4;
		}
		
		QVariant data(const QModelIndex &index, int role) const override;
		QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		template<typename T>
		void insertData(const T& data) {
			QModelIndex index = QAbstractTableModel::index(0, 0);
			if (currentSize < maxArraySize_) {
				emit QAbstractItemModel::beginInsertRows(QModelIndex{}, 0, 0);
				data_[endIndex_] = data;
				emit QAbstractItemModel::endInsertRows();
				endIndex_ = ++currentSize;
			}
			else {
				endIndex_ %= maxArraySize_;
				data_[endIndex_] = data;
				emit dataChanged(index, this->index(0,4), {});
				++endIndex_;
			}
		}

		int rowCount(const QModelIndex &parent = QModelIndex()) const override {
			Q_UNUSED(parent);
			return currentSize;
		}
	};

} // namespace SEGUIApp

