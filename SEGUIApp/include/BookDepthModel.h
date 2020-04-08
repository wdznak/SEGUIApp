#pragma once

#include <QtCore/QDebug>
#include <QtCore/QAbstractTableModel>
#include <QtGlobal>
#include <QtGui/QBrush>
#include <QtGui/QColor>

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "ANamespace.h"

namespace SEGUIApp {

	class BookDepthModel : public QAbstractTableModel
	{
	public:
		struct Order {
			_int64 size_ = 0;
			bool isBid_ = false;

			Order() = default;

			Order(_int64 size, bool isBid)
				: size_(size), isBid_(isBid) {}
		};

		struct CompareStringFloat {
			bool operator()(const std::string& a, const std::string& b) const {
				return static_cast<_int64>(100000000 * std::stod(a)) < static_cast<_int64>(100000000 * std::stod(b));
			}
		};

		using baList_t = std::vector<std::vector<std::string>>;
		using orderBook_t = std::map<std::string, struct Order, CompareStringFloat>;

	private:
		bool isBookFilled = false;
		orderBook_t orderBook_;
		_int64 firstUpdateID_ = 0;
		_int64 lastUpdateID_ = 0;

	public:
		BookDepthModel() = default;

		int columnCount(const QModelIndex& parent = QModelIndex()) const override {
			Q_UNUSED(parent);
			return 2;
		}

		QVariant data(const QModelIndex& index, int role) const override {
			if (role == Qt::DisplayRole) {
				if (index.row() > orderBook_.size() - 1)
					return QVariant();

				auto it = orderBook_.begin();
				std::advance(it, index.row());

				switch (index.column()) {
					case 0: return it->first.data();
					case 1: return it->second.size_;
					default: return QVariant{};
				}
			}

			if (role == Qt::BackgroundRole) {
				if (index.row() > orderBook_.size() - 1)
					return QVariant();

				auto it = orderBook_.begin();
				std::advance(it, index.row());

				if (it->second.isBid_) {
					QColor color("#ff5c5c");
					return QBrush(color);
				}
				else {
					QColor color("#95fa5f");
					return QBrush(color);
				}
			}

			return QVariant();
		}

		QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
			if (role != Qt::DisplayRole)
				return QVariant();

			if (orientation == Qt::Horizontal) {
				switch (section) {
					case 0: return tr("Price");
					case 1: return tr("Size");
					default: return QVariant();
				}
			}
			return QVariant();
		}

		void fillBook(const size_t updateId, const baList_t& bids, const baList_t& asks) {
			lastUpdateID_ = updateId;
			std::for_each(bids.begin(), bids.end(), [&orderBook_ = orderBook_](const std::vector<std::string>& bid) {
				orderBook_.emplace(bid[0], Order{ static_cast<_int64>(100000000 * std::stod(bid[1])), true });
			});

			std::for_each(asks.begin(), asks.end(), [&orderBook_ = orderBook_](const std::vector<std::string>& ask) {
				orderBook_.emplace(ask[0], Order{ static_cast<_int64>(100000000 * std::stod(ask[1])), false });
			});

			isBookFilled = true;
			QAbstractTableModel::beginInsertRows(QModelIndex(), 0, orderBook_.size() - 1);
			QAbstractTableModel::endInsertRows();
		}

		_int64 getLastUpdateID() const {
			return lastUpdateID_;
		}

		orderBook_t getSnapshot() const {
			return orderBook_;
		}

		int rowCount(const QModelIndex& parent = QModelIndex()) const override {
			Q_UNUSED(parent);
			return orderBook_.size();
		}
		
		void updateBidAsk(bool isBid, const std::string& price, const std::string& size, _int64 firstUpdateID, _int64 lastUpdateID) {
			firstUpdateID_ = firstUpdateID;
			lastUpdateID_ = lastUpdateID;
			_int64 fSize = static_cast<_int64>(100000000 * std::stod(size));
			
			auto it = orderBook_.find(price);
			auto row = std::distance(orderBook_.begin(), it);

			if (fSize == .0f) {
				if (it != orderBook_.end()) {
					QAbstractItemModel::beginRemoveRows(QModelIndex(), row, row);
					orderBook_.erase(it);
					QAbstractItemModel::endRemoveRows();
				}
			}
			else {
				if (it != orderBook_.end()) {
					orderBook_[price] = { fSize, isBid };
					QModelIndex index = QAbstractTableModel::index(row, 0);
					emit dataChanged(index, this->index(row, 2), {});
				}
				else {
					QAbstractItemModel::beginInsertRows(QModelIndex(), row, row);
					orderBook_[price] = { fSize, isBid };
					QAbstractItemModel::endInsertRows();
				}
			}
		}
	};

} // namespace SEGUIApp

