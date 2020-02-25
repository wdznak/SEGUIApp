#include "BookDepthHBC.h"

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCore/QDebug>

namespace SEGUIApp {

BookDepthHBC::BookDepthHBC()
{
    //![1]
    QBarSet* set0 = new QBarSet("Price");

    *set0 << 1 << 2 << 3 << 4 << 5 << 6;
    //![1]

    //![2]
    QHorizontalBarSeries* series = new QHorizontalBarSeries();
    series->append(set0);
    //![2]

    //![3]
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Simple horizontal barchart example");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    //![3]

    //![4]
    QStringList categories;
    categories << "8000" << "9000" << "8500" << "8550";
    QBarCategoryAxis* axisY = new QBarCategoryAxis();
    axisY->append(categories);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    QValueAxis* axisX = new QValueAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    axisX->applyNiceNumbers();
    //![4]

    //![5]
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    //![5]

    //![6]
    //QChartView* chartView = new QChartView(chart);
    //chartView->setRenderHint(QPainter::Antialiasing);
    setChart(chart);
    setRenderHint(QPainter::Antialiasing);
    axisY->remove("8500");
    qDebug() << categories;
    categories.removeOne("8500");
    qDebug() << categories;
    categories.removeOne("9000");
}

BookDepthHBC::~BookDepthHBC()
{
}

} // namespace seguiapp