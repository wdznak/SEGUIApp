#include "OfflineModeWidget.h"

void SEGUIApp::OfflineModeWidget::initBook(const QString& path, bool isInit) {
	if (from_ > to_) {
		postMessage("Invalid time range!", Message::M_ERROR);
		return;
	}

	QString infoMessage("Initiating with: ");
	infoMessage += path;
	postMessage(infoMessage, Message::M_INFO);
	// !!! CHANGE false to isInit !!!
	if (fileDataParser_.initBook(path, false, from_, to_)) {
		dataAggregator_.init(QDateTime::fromMSecsSinceEpoch(std::max(from_, fileDataParser_.getStartTime())));
	}
	else {
		postMessage("Can't initiate file parser!", Message::M_ERROR);
	};
}

void SEGUIApp::OfflineModeWidget::from(const QDateTime& dateTime) {
	QTime time(dateTime.time().hour(), dateTime.time().minute());
	QDateTime zeroedDateTime(dateTime.date(), time);

	from_ = zeroedDateTime.toMSecsSinceEpoch();
}

void SEGUIApp::OfflineModeWidget::openFileDialog() {
	/*auto url = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
	"D://Binance BTC-USDT");*/
	QString url = "D://Binance BTC-USDT";

	if ("" == url) return;

	QDirIterator it(url, QDir::Files);
	while (it.hasNext()) {
		it.next();
		fileDataModel_.addFile(it.fileInfo());
	}

	fileDataModel_.updateView();
}

void SEGUIApp::OfflineModeWidget::to(const QDateTime& dateTime) {
	QTime time(dateTime.time().hour(), dateTime.time().minute());
	QDateTime zeroedDateTime(dateTime.date(), time);

	to_ = zeroedDateTime.toMSecsSinceEpoch();
}
