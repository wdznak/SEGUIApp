#pragma once

#include <QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QDirIterator>
#include <QtWidgets/QFileDialog>
#include <QWidget>

#include "ui_OfflineModeWidget.h"

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
		DataAggregator dataAggregator_{ bookDepthModel_, 15 };
		FileDataModel fileDataModel_;
		FileDataParser fileDataParser_{ bookDepthModel_ };

	public:
		OfflineModeWidget() {
			offlineMode_.setupUi(this);
			offlineMode_.dateTimeEdit->setDateTime(QDateTime::currentDateTime());
			offlineMode_.fileTable->setModel(&fileDataModel_);
			
			if (fileDataParser_.initBook("D:\\Binance BTC-USDT\\2019-05-10_211059_Binance_USDT-BTC_INIT.txt")) {
				dataAggregator_.init(fileDataParser_.getStartTime());
			};

			while (fileDataParser_.hasNext()) {
				fileDataParser_.next();
			}

			offlineMode_.depthTable->setModel(&bookDepthModel_);

			connect(offlineMode_.selectFolderB, &QPushButton::clicked, this, &OfflineModeWidget::openFileDialog);
			connect(offlineMode_.readNext, &QPushButton::clicked, this, &OfflineModeWidget::readNext);
		}

		void openFileDialog() {
			auto url = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
				"C://CryptoData");
			
			if ("" == url) return;

			QDirIterator it(url, QDir::Files);
			while (it.hasNext()) {
				it.next();
				fileDataModel_.addFile(it.fileInfo());
			}

			fileDataModel_.updateView();
		}

		void readNext() {
			fileDataParser_.next();
		}
	};

} // namespace SEGUIApp