#pragma once

#include <QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QDirIterator>
#include <QtWidgets/QFileDialog>
#include <QWidget>

#include "ui_OfflineModeWidget.h"

#include "BookDepthModel.h"
#include "FileDataModel.h"
#include "FileDataParser.h"

namespace SEGUIApp {

	class OfflineModeWidget : public QWidget
	{
		Q_OBJECT
	private:
		Ui::OfflineModeWidget offlineMode_;

		BookDepthModel bookDepthModel_;
		FileDataModel fileDataModel_;
		FileDataParser fileDataParser_{ bookDepthModel_ };

	public:
		OfflineModeWidget() {
			offlineMode_.setupUi(this);
			offlineMode_.dateTimeEdit->setDateTime(QDateTime::currentDateTime());
			offlineMode_.fileTable->setModel(&fileDataModel_);
			
			fileDataParser_.openFile("D:\\Binance BTC-USDT\\2019-05-10_211059_Binance_USDT-BTC_INIT.txt");
			offlineMode_.depthTable->setModel(&bookDepthModel_);
			qDebug() << QDateTime::currentMSecsSinceEpoch();
			qDebug() << QDateTime::fromMSecsSinceEpoch(1557522661553);

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