#include "FileDataModel.h"
#include "..\include\FileDataModel.h"

#include <QtCore/QDebug>

namespace SEGUIApp {

	void FileDataModel::addFile(const QFileInfo& qFileInfo) {
		FileInfo fileInfo;
		if (parseFileName(qFileInfo, fileInfo) && exchangeList_.at(exchange_) == fileInfo.exchangeName) {
			fileInfo.name = qFileInfo.fileName();
			fileInfo.path = qFileInfo.absoluteFilePath();
			fileList_.insert(fileInfo);
		}
	}

	QModelIndex FileDataModel::createIndex(int row, int column) {
		return QAbstractTableModel::createIndex(row, column);
	}

	QVariant FileDataModel::data(const QModelIndex& index, int role) const {
		if (role == Qt::DisplayRole) {
			if (index.row() > fileList_.size())
				return QVariant{};

			auto it = fileList_.begin();
			std::advance(it, index.row());

			switch (index.column()) {
			case 0: return it->isInit ? "INIT" : "";
			case 1: return it->name;
			case 2: return it->dateTime;
			case 3: return it->path;
			default: return QVariant{};
			}
		}
		return QVariant();
	}

	QVariant FileDataModel::headerData(int section, Qt::Orientation orientation, int role) const {
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

	int FileDataModel::rowCount(const QModelIndex& parent) const {
		Q_UNUSED(parent);
		return fileList_.size();
	}

	void FileDataModel::updateView() {
		QAbstractTableModel::beginInsertRows(QModelIndex(), 0, fileList_.size() - 1);
		QAbstractTableModel::endInsertRows();
	}

	bool FileDataModel::parseFileName(const QFileInfo& qFileInfo, FileInfo& fileInfo) {
		QString s = qFileInfo.fileName();
		QRegularExpressionMatch match = regex_.match(qFileInfo.fileName());

		if (match.hasMatch()) {
			int h, m, s, y, mo, d;
			h = match.captured(4).mid(0, 2).toInt();
			m = match.captured(4).mid(2, 2).toInt();
			s = match.captured(4).mid(4, 2).toInt();
			
			y = match.captured(1).toInt();
			mo = match.captured(2).toInt();
			d = match.captured(3).toInt();

			fileInfo.dateTime.setDate(QDate(y, mo, d));
			fileInfo.dateTime.setTime(QTime(h, m, s));
			fileInfo.exchangeName = match.captured(5);
			fileInfo.pair = match.captured(6);
			
			if (match.captured(7) == "_INIT") {
				fileInfo.isInit = true;
			}

			return true;
		}

		return false;
	}

} // namespace SEGUIApp
