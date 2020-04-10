#pragma once

#include <algorithm>

#include <QCandlestickSeries>
#include <QChart>
#include <QChartView>
#include <QtConcurrent/QtConcurrent>
#include <QtCore/QFutureWatcher>
#include <QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QDirIterator>
#include <QtWidgets/QFileDialog>
#include <QWidget>

#include "ui_OfflineModeWidget.h"

#include "ANamespace.h"
#include "BasicStatistics.h"
#include "BookDepthModel.h"
#include "DataAggregator.h"
#include "FileDataModel.h"
#include "FileDataParser.h"
#include "StatsTableModel.h"

namespace SEGUIApp {

	class OfflineModeWidget : public QWidget
	{
		Q_OBJECT
	private:
		Ui::OfflineModeWidget offlineMode_;

		BookDepthModel bookDepthModel_;
		QModelIndex currentFileIndex_;
		std::unique_ptr<DataAggregator> dataAggregator_;
		FileDataModel fileDataModel_;
		FileDataParser fileDataParser_{ bookDepthModel_ };
		_int64 from_ = 1585586700000;
		_int64 to_   = 1585596600000;
		QListWidget* messageList_;
		StatsTableModel statsTableModel_;
		std::unique_ptr<StatisticsI> statistics_;

	public:
		OfflineModeWidget() {
			offlineMode_.setupUi(this);

			statistics_ = std::make_unique<BasicStatistics>();
			dataAggregator_ = std::make_unique<DataAggregator>(bookDepthModel_, 30, statistics_.get());

			messageList_ = offlineMode_.messageList;
			offlineMode_.dateTimeFrom->setDateTime(QDateTime::currentDateTime());
			offlineMode_.dateTimeTo->setDateTime(QDateTime::currentDateTime());
			offlineMode_.fileTable->setModel(&fileDataModel_);
			offlineMode_.depthTable->setModel(&bookDepthModel_);

			connect(offlineMode_.selectFolderB, &QPushButton::clicked, this, &OfflineModeWidget::openFileDialog);
			connect(offlineMode_.readNext, &QPushButton::clicked, this, &OfflineModeWidget::readNext);
			connect(offlineMode_.dateTimeFrom, &QDateTimeEdit::dateTimeChanged, this, &OfflineModeWidget::from);
			connect(offlineMode_.dateTimeTo, &QDateTimeEdit::dateTimeChanged, this, &OfflineModeWidget::to);
			connect(&fileDataParser_, &FileDataParser::depthUpdated, dataAggregator_.get(), &DataAggregator::onDepthUpdated);
			connect(&fileDataParser_, &FileDataParser::trade, dataAggregator_.get(), &DataAggregator::onTrade, Qt::ConnectionType::DirectConnection);
			
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
			QFutureWatcher<void>* watcher = new QFutureWatcher<void>();
			QFuture<void> future = QtConcurrent::run([&]() {
				while (fileDataParser_.hasNext()) {
					fileDataParser_.next();
				}

				while (!fileDataParser_.reachedEnd() && getNextFile()) {
					while (fileDataParser_.hasNext()) {
						fileDataParser_.next();
					}
				}

				dataAggregator_->printDebug();
			});
			connect(watcher, &QFutureWatcher<void>::finished, [this]() {
				postMessage("Reading is finished!", Message::M_INFO);
				BasicStatistics* bs = (BasicStatistics*)statistics_.get();
				statsTableModel_.fillModel(bs->getIntervals());
				offlineMode_.statsTable->setModel(&statsTableModel_);
				});

			watcher->setFuture(future);
		}

		void to(const QDateTime& dateTime);
	};

} // namespace SEGUIApp