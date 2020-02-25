#pragma once

#include <set>

#include <QtCore/QAbstractTableModel>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QRegularExpression>

#include "ANamespace.h"

namespace SEGUIApp {

	class FileDataModel : public QAbstractTableModel
	{
	private:
		struct FileInfo {
			QDateTime dateTime;
			QString exchangeName;
			QString name;
			QString pair;
			QString path;
			bool isInit = false;
		};

		struct fi_compare {
			bool operator() (const FileInfo& lhs, const FileInfo& rhs) const {
				return lhs.dateTime < rhs.dateTime;
			}
		};

		Exchange exchange_ = Exchange::NONE;
		const std::map<Exchange, QString> exchangeList_{ { Exchange::BINANCE, "Binance" },
												         { Exchange::NONE, "" } 
		};
		std::set<FileInfo, fi_compare> fileList_;
		// Pattern (year)(month)(day)(time h:m:s)(exchange name)(coin pair)(INIT or regular file)
		// e.g.    (2020)(02)   (12) (123045)    (Binance)      (BTC-USDT) [(_INIT) | ()]
		QRegularExpression regex_{ "^(\\d{4})-(\\d{2})-(\\d{2})_(\\d{6})_([A-Za-z]+)_([A-Za-z]{1,5}-[A-Za-z]{1,5})(_INIT|).txt$" };

	public:
		FileDataModel(Exchange exchange = Exchange::BINANCE) : exchange_(exchange) {};

		int columnCount(const QModelIndex& parent = QModelIndex()) const override {
			Q_UNUSED(parent);
			return 3;
		}

		void addFile(const QFileInfo& qFileInfo);

		QVariant data(const QModelIndex& index, int role) const override { 
			if (role == Qt::DisplayRole) {
				if (index.row() > fileList_.size())
					return QVariant{};

				auto it = fileList_.begin();
				std::advance(it, index.row());

				switch (index.column()) {
				case 0: return it->isInit ? "INIT" : "";
				case 1: return it->name;
				case 2: return it->dateTime;
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
				case 0: return tr("Initial");
				case 1: return tr("File Name");
				case 2: return tr("Date");
				default: return QVariant();
				}
			}
			return QVariant();
		}

		int rowCount(const QModelIndex& parent = QModelIndex()) const override {
			Q_UNUSED(parent);
			return fileList_.size();
		}

		void updateView() {
			QAbstractTableModel::beginInsertRows(QModelIndex(), 0, fileList_.size());
			QAbstractTableModel::endInsertRows();
		}

	private:
		bool parseFileName(const QFileInfo& qFileInfo, FileInfo& fileInfo);
	};

} // namespace SEGUIApp

