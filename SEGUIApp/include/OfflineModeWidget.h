#pragma once

#include <QDebug>
#include <QtCore/QDirIterator>
#include <QtWidgets/QFileDialog>
#include <QWidget>

#include "ui_OfflineModeWidget.h"

#include "FileDataModel.h"

namespace SEGUIApp {

	class OfflineModeWidget : public QWidget
	{
		Q_OBJECT
	private:
		Ui::OfflineModeWidget offlineMode_;

		FileDataModel fileDataModel_;

	public:
		OfflineModeWidget() {
			offlineMode_.setupUi(this);
			offlineMode_.dateTimeEdit->setDateTime(QDateTime::currentDateTime());
			offlineMode_.fileTable->setModel(&fileDataModel_);

			connect(offlineMode_.selectFolderB, &QPushButton::clicked, this, &OfflineModeWidget::openFileDialog);
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
	};

} // namespace SEGUIApp