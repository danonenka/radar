#include "radar_data.h"
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <cstring>

bool RadarData::loadFromFile(const QString &filename) {
    QFile file(filename);
    if (!file.exists()) {
        qCritical() << "Файл не найден!";
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Не удалось открыть файл!" << file.errorString();
        return false;
    }

    if (file.read(reinterpret_cast<char*>(&header), sizeof(RadarHeader)) != sizeof(RadarHeader)) {
        qCritical() << "Ошибка чтения заголовка!";
        return false;
    }

    if (strncmp(header.head, "HEAD", 4) != 0 || strncmp(header.data_id, "DATA", 4) != 0) {
        qCritical() << "Неверный формат файла!";
        return false;
    }

    start_angle = header.start_angle_int + header.start_angle_frac / 1000.0;
    end_angle = header.end_angle_int + header.end_angle_frac / 1000.0;
    step_angle = header.step_angle_int + header.step_angle_frac / 1000.0;

    int N = static_cast<int>((end_angle - start_angle) / step_angle) + 1;

    QVector<int16_t> rawAmplitudes(N);
    if (file.read(reinterpret_cast<char*>(rawAmplitudes.data()), N * sizeof(int16_t)) != N * sizeof(int16_t)) {
        qCritical() << "Ошибка чтения данных!";
        return false;
    }

    amplitudes.resize(N);
    for (int i = 0; i < N; ++i) {
        amplitudes[i] = static_cast<float>(rawAmplitudes[i]) / 32768.0f;
    }

    file.close();
    return true;
}

void RadarData::printData() const {
    qDebug() << "Файл успешно прочитан!";
    qDebug() << "Начальный угол:" << start_angle << "градусов";
    qDebug() << "Конечный угол:" << end_angle << "градусов";
    qDebug() << "Шаг угла:" << step_angle << "градусов";
    qDebug() << "Число позиций:" << amplitudes.size();
    qDebug() << "Пример амплитуд (первые 10):";
    for (int i = 0; i < qMin(10, amplitudes.size()); ++i) {
        qDebug() << amplitudes[i];
    }
}
