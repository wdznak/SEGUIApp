#include "FileDataModel.h"
#include "..\include\FileDataModel.h"

namespace SEGUIApp {

	void FileDataModel::addFile(const QFileInfo& qFileInfo) {
		FileInfo fileInfo;
		if (parseFileName(qFileInfo, fileInfo) && exchangeList_.at(exchange_) == fileInfo.exchangeName) {
			fileInfo.name = qFileInfo.fileName();
			fileInfo.path = qFileInfo.absoluteFilePath();
			fileList_.insert(fileInfo);
		}
	}

	bool FileDataModel::parseFileName(const QFileInfo& qFileInfo, FileInfo& fileInfo) {
		QString s = qFileInfo.fileName();
		QRegularExpressionMatch match = regex_.match(qFileInfo.fileName());

		if (match.hasMatch()) {
			int h, m, s;
			h = match.captured(4).mid(0, 2).toInt();
			m = match.captured(4).mid(2, 2).toInt();
			s = match.captured(4).mid(4, 2).toInt();

			fileInfo.dateTime.setDate(QDate(match.captured(1).toInt(), match.captured(2).toInt(), match.captured(3).toInt()));
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
