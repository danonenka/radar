#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "radar_data.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectFile();
    void onThresholdChanged(double threshold);

private:
    Ui::MainWindow *ui;
    RadarData radar;
    double threshold = 0.5;
    QVector<QPair<double, float>> detectedObjects;
    void plotGraph();
    void updateTable(const QVector<QPair<double, float>>& detectedObjects);
};

#endif // MAINWINDOW_H
