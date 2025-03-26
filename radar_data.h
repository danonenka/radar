#ifndef RADAR_DATA_H
#define RADAR_DATA_H

#include <QVector>
#include <QString>

struct RadarHeader {
    char head[4];
    int16_t start_angle_int;
    int16_t start_angle_frac;
    int16_t end_angle_int;
    int16_t end_angle_frac;
    int16_t step_angle_int;
    int16_t step_angle_frac;
    char data_id[4];
};

class RadarData {
public:
    bool loadFromFile(const QString &filename);
    void printData() const;

    double getStartAngle() const { return start_angle; }
    double getEndAngle() const { return end_angle; }
    double getStepAngle() const { return step_angle; }
    const QVector<float>& getAmplitudes() const { return amplitudes; }

private:
    RadarHeader header;
    double start_angle = 0;
    double end_angle = 0;
    double step_angle = 0;
    QVector<float> amplitudes;
};

#endif // RADAR_DATA_H
