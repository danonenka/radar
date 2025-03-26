#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->btnSelectFile, &QPushButton::clicked, this, &MainWindow::onSelectFile);
    connect(ui->thresholdSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onThresholdChanged);

    ui->thresholdSpinBox->setRange(0, 1);
    ui->thresholdSpinBox->setSingleStep(0.01);
    ui->thresholdSpinBox->setValue(threshold);

    // Настраиваем таблицу
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels({"Угол (°)", "Амплитуда"});
    resize(1600,1400);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onSelectFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Binary Files (*.bin)");

    if (radar.loadFromFile(filename)) {
        plotGraph();
        updateTable(detectedObjects);  // Обновляем таблицу

        QString output;
        output += QString("Файл: %1\n").arg(filename);
        output += QString("Начальный угол: %1 градусов\n").arg(radar.getStartAngle());
        output += QString("Конечный угол: %1 градусов\n").arg(radar.getEndAngle());
        output += QString("Шаг угла: %1 градусов\n").arg(radar.getStepAngle());
        output += QString("Число позиций: %1\n").arg(radar.getAmplitudes().size());

        ui->textEdit->setPlainText(output);
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить файл!");
    }
}

void MainWindow::onThresholdChanged(double newThreshold) {
    threshold = newThreshold;
    plotGraph();  // Обновляем график и `detectedObjects`
}


void MainWindow::updateTable(const QVector<QPair<double, float>>& detectedObjects) {
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(detectedObjects.size());

    for (int i = 0; i < detectedObjects.size(); ++i) {
        QTableWidgetItem *angleItem = new QTableWidgetItem(QString::number(detectedObjects[i].first, 'f', 3));
        QTableWidgetItem *amplitudeItem = new QTableWidgetItem(QString::number(detectedObjects[i].second, 'f', 3));

        angleItem->setTextAlignment(Qt::AlignCenter);
        amplitudeItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget->setItem(i, 0, angleItem);
        ui->tableWidget->setItem(i, 1, amplitudeItem);
    }
}

void MainWindow::plotGraph() {
    QVector<double> x, y, xPeaks, yPeaks;
    detectedObjects.clear();  // Очищаем список объектов перед обновлением

    double startAngle = radar.getStartAngle();
    double stepAngle = radar.getStepAngle();
    const QVector<float>& amplitudes = radar.getAmplitudes();

    if (amplitudes.isEmpty()) {
        return;
    }

    for (int i = 0; i < amplitudes.size(); ++i) {
        double angle = startAngle + i * stepAngle;
        x.append(angle);
        y.append(amplitudes[i]);

        // Проверяем локальный максимум, превышающий порог
        if (i > 0 && i < amplitudes.size() - 1) {
            if (amplitudes[i] > threshold && amplitudes[i] > amplitudes[i - 1] && amplitudes[i] > amplitudes[i + 1]) {
                xPeaks.append(angle);
                yPeaks.append(amplitudes[i]);
                detectedObjects.append(qMakePair(angle, amplitudes[i]));  // Заполняем список объектов
            }
        }
    }

    ui->customPlot->clearGraphs();

    //Основной график амплитуд
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(x, y);
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue));

    // Линия порога
    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setData({x.first(), x.last()}, {threshold, threshold});
    ui->customPlot->graph(1)->setPen(QPen(Qt::red, 2, Qt::DashLine));

    // Выделение локальных максимумов (найденных объектов)
    if (!xPeaks.isEmpty()) {
        ui->customPlot->addGraph();
        ui->customPlot->graph(2)->setData(xPeaks, yPeaks);
        ui->customPlot->graph(2)->setPen(QPen(Qt::green));
        ui->customPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
        ui->customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
    }

    ui->customPlot->xAxis->setLabel("Угол азимута");
    ui->customPlot->yAxis->setLabel("Амплитуда сигнала");

    // Добавляем возможность увеличения и перемещения графика
    ui->customPlot->setInteraction(QCP::iRangeZoom, true);
    ui->customPlot->setInteraction(QCP::iRangeDrag, true);

    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();

    // Обновляем таблицу обнаруженных объектов
    updateTable(detectedObjects);
}




