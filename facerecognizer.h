#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

#include <QImage>
#include <QByteArray>
#include <QVector>
#include <QDataStream>

class FaceRecognizer {
public:
    FaceRecognizer();

    // Extract LBP histogram features from face image
    QVector<double> extractFeatures(const QImage& faceImage);

    // Compare two feature vectors using chi-square distance
    double compareFeatures(const QVector<double>& feat1, const QVector<double>& feat2);

    // Serialize features for database storage
    QByteArray serializeFeatures(const QVector<double>& features);
    QVector<double> deserializeFeatures(const QByteArray& data);

    // Recognition threshold
    static constexpr double RECOGNITION_THRESHOLD = 0.35;

private:
    QVector<double> computeLBPHistogram(const QImage& grayImage);

};

#endif // FACERECOGNIZER_H
