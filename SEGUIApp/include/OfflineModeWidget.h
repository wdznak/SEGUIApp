#pragma once

#include <algorithm>

#include <QtConcurrent/QtConcurrent>
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
		_int64 from_ = 1585586700000;
		_int64 to_   = 1585596600000;
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
			connect(&fileDataParser_, &FileDataParser::trade, &dataAggregator_, &DataAggregator::onTrade, Qt::ConnectionType::DirectConnection);
			
			connect(offlineMode_.fileTable, &QAbstractItemView::doubleClicked, this, &OfflineModeWidget::onFileRowDClicked);
		}

	private:
		bool getNextFile() {
			const int nextRow = currentFileIndex_.row() + 1;
			if (nextRow >= fileDataModel_.rowCount()) {
				postMessage("No more files to process. Data might be incomplete.", Message::M_INFO);
				return false;
			}
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

		void initBook(const QString& path, bool isInit);

		void from(const QDateTime& dateTime);

		void openFileDialog();

		void onFileRowDClicked(const QModelIndex& index) {
			if (index.isValid()) {
				QModelIndex init = fileDataModel_.createIndex(index.row(), 0);
				QModelIndex filePath = fileDataModel_.createIndex(index.row(), 3);
				bool isInit = true;

				if (fileDataModel_.data(init, Qt::DisplayRole).toString() != "INIT") {
					postMessage("Not INIT file! Book depth will not be updated!", Message::M_INFO);
					isInit = false;
				}

				currentFileIndex_ = init;
				
				initBook(fileDataModel_.data(filePath, Qt::DisplayRole).toString(), isInit);
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
			QFuture<void> future = QtConcurrent::run([&]() {
				while (fileDataParser_.hasNext()) {
					fileDataParser_.next();
				}

				while (!fileDataParser_.reachedEnd() && getNextFile()) {
					while (fileDataParser_.hasNext()) {
						fileDataParser_.next();
					}
				}

				dataAggregator_.printDebug();
			});
		}

		void readNext1() {
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