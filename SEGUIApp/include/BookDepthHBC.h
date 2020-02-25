#pragma once

#include <QtCharts/QChartView>
#include <QtCore/QString>

namespace SEGUIApp {

QT_CHARTS_USE_NAMESPACE

	class BookDepthHBC: public QChartView
	{
		Q_OBJECT

	public:
		BookDepthHBC();
		~BookDepthHBC();

		void test() {
		}
	};

} // namespace seguiapp
