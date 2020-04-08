#pragma once

#include <QtCore/QAbstractTableModel>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtGlobal>
#include <QtGui/QBrush>
#include <QtGui/QColor>

#include <map>
#include <string>
#include <vector>

#include "ANamespace.h"
#include "BasicStatistics.h"

namespace SEGUIApp {

	class StatsTableModel : public QAbstractTableModel
	{
	private:
		struct comp {
			bool operator()(const std::pair<_int64, double>& a, const std::pair<_int64, double>& b) const {
				return a.second > b.second;
			}
		};

		std::vector<std::vector<QVariant>> modelData_;
	public: 
		StatsTableModel() = default;

		int columnCount(const QModelIndex& parent = QModelIndex()) const override {
			Q_UNUSED(parent);
			return 8;
		}

		QVariant data(const QModelIndex& index, int role) const override {
			if (role == Qt::DisplayRole) {
				if (index.row() > modelData_.size() - 1)
					return QVariant();

				return modelData_[index.row()][index.column()];
			}

			return QVariant();
		}

		QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
			if (role != Qt::DisplayRole)
				return QVariant();

			if (orientation == Qt::Horizontal) {
				switch (section) {
				case 0: return tr("Time");
				case 1: return tr("B/S diff");
				case 2: return tr("Big bought");
				case 3: return tr("Big sold");
				case 4: return tr("Diff");
				case 5: return tr("Fish bought");
				case 6: return tr("Fish sold");
				case 7: return tr("Diff");
				default: return QVariant();
				}
			}
			return QVariant();
		}

		int rowCount(const QModelIndex& parent = QModelIndex()) const override {
			Q_UNUSED(parent);
			return modelData_.size();
		}

		void fillModel(const std::vector<BasicStatistics::Interval>& intervals) {
			std::unordered_map<_int64, double> value;
			const int period = 1;

			if (intervals.size() < period) {
				return;
			}

			int j = 0;
			for (int i = 0; i < intervals.size(); ++i) {
				for (j = i; j < period + i; ++j) {
					for (auto f : intervals[j].tradeStats.data.users) {
						value[f.first] += f.second.totalAmount;
					}
				}
				std::vector<QVariant> data(8);
				qDebug() << QDateTime::fromMSecsSinceEpoch(intervals[j].startTime);
				data[0] = QDateTime::fromMSecsSinceEpoch(intervals[j].startTime);
				stats(value, data);
				modelData_.push_back(std::move(data));
				value.clear();
				if (period + i >= intervals.size() - 1) return;
			}

		}

	private:
		void stats(const std::unordered_map<_int64, double>& value, std::vector<QVariant>& data) {
			
			std::vector<std::pair<_int64, double>> elems(value.begin(), value.end());
			std::sort(elems.begin(), elems.end(), comp());

			int users = 10;
			double totalBuy = 0;
			double totalSell = 0;

			int totalUsers, totalBuyers, totalSellers;
			const auto dcomp = [](const std::pair<_int64, double>& a, double b) -> bool {
				return a.second > b;
			};

			auto mid = std::lower_bound(elems.begin(), elems.end(), 0, dcomp);

			totalUsers = elems.size();
			totalBuyers = std::distance(elems.begin(), mid);
			totalSellers = std::distance(mid, elems.end());

			qDebug() << "Total buyers--------------- " << totalBuyers << " --------------------";
			for (int i = 0; i <= totalBuyers; ++i) {
				totalBuy += elems[i].second;
			}
			qDebug() << totalBuy << "\n";

			qDebug() << "Total sellers----------------- " << totalSellers << " ------------------";
			for (int i = elems.size() - 1; i > elems.size() - totalBuyers; --i) {
				totalSell += elems[i].second;
			}
			qDebug() << totalSell << "\n\n";
			double perDiff = ((totalBuyers - totalSellers) / ((totalBuyers + totalSellers) / 2.0f)) * 100;
			qDebug() << "user % diff buy/sell: " << perDiff << "\n";
			data[1] = perDiff;

			const double perTreshold = 0.05f;
			int valTreshold = totalBuyers * perTreshold;
			double currAmountB = 0;
			int topBuyUCount = 0;
			int topSellUCount = 0;

			for (int i = 0; i <= valTreshold; ++i) {
				currAmountB += elems[i].second;
			}

			qDebug() << "Top " << valTreshold << " Buyers ------------------------------";
			qDebug() << "Bought:         " << currAmountB;
			qDebug() << "The rest bought:" << (totalBuy - currAmountB) << "\n";
			data[2] = currAmountB;
			data[5] = (totalBuy - currAmountB);

			valTreshold = totalSellers * perTreshold;
			double currAmountS = 0;
			auto rmid = elems.rbegin();

			std::advance(rmid, totalSellers);

			for (int i = elems.size() - 1; i > elems.size() - 1 - valTreshold; --i) {
				currAmountS += std::abs(elems[i].second);
			}

			qDebug() << "Top " << valTreshold << " Sellers ------------------------------";
			qDebug() << "Sold:         " << currAmountS;
			data[3] = currAmountS;
			qDebug() << "The rest sold:" << (totalSell + currAmountS) << "\n";
			data[6] = (totalSell + currAmountS);

			qDebug() << "Whales diff: " << currAmountB - currAmountS;
			data[4] = currAmountB - currAmountS;
			qDebug() << "Fish diff:   " << (totalBuy - currAmountB) - std::abs(totalSell + currAmountS);
			data[7] = (totalBuy - currAmountB) - std::abs(totalSell + currAmountS);
			qDebug() << "Total Buy:  " << totalBuy;
			qDebug() << "Total Sell: " << totalSell;
		}
	};

} // namespace SEGUIApp