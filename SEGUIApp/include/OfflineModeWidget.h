#pragma once

#include <algorithm>

#include <QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QDirIterator>
#include <QtWidgets/QFileDialog>
#include <QWidget>

#include "ui_OfflineModeWidget.h"

#include "ANamespace.h"
#include "BookDepthModel.h"
#include "DataAggregator.h"
#include "FileDataModel.h"
#include "FileDataParser.h"

namespace SEGUIApp {

	class OfflineModeWidget : public QWidget
	{
		Q_OBJECT
	private:
		Ui::OfflineModeWidget offlineMode_;

		BookDepthModel bookDepthModel_;
		QModelIndex currentFileIndex_;
		DataAggregator dataAggregator_{ bookDepthModel_, 15 };
		FileDataModel fileDataModel_;
		FileDataParser fileDataParser_{ bookDepthModel_ };
		_int64 from_ = 1557523800000;
		_int64 to_   = 1557533700000;
		QListWidget* messageList_;

	public:
		OfflineModeWidget() {
			offlineMode_.setupUi(this);
			messageList_ = offlineMode_.messageList;
			offlineMode_.dateTimeFrom->setDateTime(QDateTime::currentDateTime());
			offlineMode_.dateTimeTo->setDateTime(QDateTime::currentDateTime());
			offlineMode_.fileTable->setModel(&fileDataModel_);
			offlineMode_.depthTable->setModel(&bookDepthModel_);

			connect(offlineMode_.selectFolderB, &QPushButton::clicked, this, &OfflineModeWidget::openFileDialog);
			connect(offlineMode_.readNext, &QPushButton::clicked, this, &OfflineModeWidget::readNext);
			connect(offlineMode_.dateTimeFrom, &QDateTimeEdit::dateTimeChanged, this, &OfflineModeWidget::from);
			connect(offlineMode_.dateTimeTo, &QDateTimeEdit::dateTimeChanged, this, &OfflineModeWidget::to);
			connect(&fileDataParser_, &FileDataParser::depthUpdated, &dataAggregator_, &DataAggregator::onDepthUpdated);
			connect(&fileDataParser_, &FileDataParser::trade, &dataAggregator_, &DataAggregator::onTrade);
			
			connect(offlineMode_.fileTable, &QAbstractItemView::doubleClicked, this, &OfflineModeWidget::onFileRowDClicked);
		}

	private:
		bool getNextFile() {
			const int nextRow = currentFileIndex_.row() + 1;
			QModelIndex init = fileDataModel_.createIndex(nextRow, 0);
			QModelIndex filePath = fileDataModel_.createIndex(nextRow, 3);
			currentFileIndex_ = init;

			if (init.isValid() && fileDataModel_.data(init, Qt::DisplayRole).toString() != "INIT") {
				QString infoMessage("Loading next file: ");
				infoMessage += fileDataModel_.data(filePath, Qt::DisplayRole).toString();
				postMessage(infoMessage, Message::M_INFO);
				return fileDataParser_.loadFile(fileDataModel_.data(filePath, Qt::DisplayRole).toString());
			}

			return false;
		}

		void initBook(const QString& path);

		void from(const QDateTime& dateTime);

		void openFileDialog();

		void onFileRowDClicked(const QModelIndex& index) {
			if (index.isValid()) {
				QModelIndex init = fileDataModel_.createIndex(index.row(), 0);
				QModelIndex filePath = fileDataModel_.createIndex(index.row(), 3);

				if (fileDataModel_.data(init, Qt::DisplayRole).toString() != "INIT") {
					qErrnoWarning("It is not an INIT file. Book depth would be invalid!");
					return;
				}

				currentFileIndex_ = init;
				
				initBook(fileDataModel_.data(filePath, Qt::DisplayRole).toString());
			}
			else {
				qDebug() << "CLICKED but err!";
			}
		}

		void postMessage(QString message, Message type) {
			if (Message::M_ERROR == type) {
				new QListWidgetItem(QIcon(messageIco.at(Message::M_ERROR)), message, messageList_);
			}
			else if (Message::M_WARNING == type) {
				new QListWidgetItem(QIcon(messageIco.at(Message::M_WARNING)), message, messageList_);
			}
			else if (Message::M_INFO == type) {
				new QListWidgetItem(QIcon(messageIco.at(Message::M_INFO)), message, messageList_);
			}
		}

		void readNext() {
			while (fileDataParser_.hasNext()) {
				fileDataParser_.next();
			}

			while (!fileDataParser_.reachedEnd() && getNextFile()) {
				while (fileDataParser_.hasNext()) {
					fileDataParser_.next();
				}
			}
			
			dataAggregator_.printDebug();
		}

		void to(const QDateTime& dateTime);
	};

} // namespace SEGUIApp