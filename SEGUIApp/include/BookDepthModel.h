#pragma once

#include <QtCore/QDebug>
#include <QtGlobal>

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <QtCore/QAbstractTableModel>

namespace SEGUIApp {

	class BookDepthModel : public QAbstractTableModel
	{
	public:
		using baList_t = std::vector<std::vector<std::string>>;

	private:
		struct compare {
			bool operator()(const std::string& lhs, const std::string& rhs) const {
				return std::stof(lhs) > std::stof(rhs);
			}
		};

		std::map<float, float> orderBook_;
		bool isBookFilled = false;
		size_t updateId_ = 0;
		float midPoint_;

	public:
		BookDepthModel() {}

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
					case 0: return it->first;
					case 1: return it->second;
					default: return QVariant{};
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
			std::for_each(bids.begin(), bids.end(), [&orderBook_ = orderBook_](const std::vector<std::string>& bid) {
				orderBook_.emplace(std::stof(bid[0]), std::stof(bid[1]));
			});

			if (!orderBook_.empty()) {
				midPoint_ = orderBook_.begin()->first;
			}

			std::for_each(asks.begin(), asks.end(), [&orderBook_ = orderBook_](const std::vector<std::string>& ask) {
				orderBook_.emplace(std::stof(ask[0]), std::stof(ask[1]));
			});

			isBookFilled = true;
		}

		int rowCount(const QModelIndex& parent = QModelIndex()) const override {
			Q_UNUSED(parent);
			return orderBook_.size();
		}
		// !!!!!!!!!!!!!! UPDATE TABLEVIEW ONLY WHEN FULL BATCH OF BIDS AND ASKS 
		// HAS BEEN UPDATED. DONT DO UPDATE PER BID CAUSE DATA AROUND MIDPOINT
		// CAN BE SKEWED
		void updateAsks(const std::string& price, const std::string& size) {
			float fPrice = std::stof(price);
			float fSize	 = std::stof(size);

			if (midPoint_ < fPrice) {
				midPoint_ = fPrice;
			}
			else if (midPoint_ == fPrice && fSize == .0f) {
				auto it = orderBook_.find(fPrice);

				if (it != orderBook_.end() && ++it != orderBook_.end()) {
					midPoint_ = it->first;
				}
				else {
					midPoint_ = -1;
				}
			}

			if (fSize == .0f) {
				orderBook_.erase(fPrice);
			}
			else {
				orderBook_[fPrice] = fSize;
			}
		}

		void updateBids(const std::string& price, const std::string& size) {
			float fPrice = std::stof(price);
			float fSize = std::stof(size);

			if (fSize == .0f) {
				orderBook_.erase(fPrice);
			}
			else {
				orderBook_[fPrice] = fSize;
			}

			if (midPoint_ > fPrice) {
				auto it = orderBook_.find(fPrice);
				if (it != orderBook_.end() && ++it != orderBook_.end()) {
					midPoint_ = it->first;
				}
				else {
					midPoint_ = -1;
				}
			}
		}
	};

} // namespace SEGUIApp

