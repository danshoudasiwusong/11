#include "facerecognizer.h"
#include <QImage>
#include <QIODevice>
#include <cmath>

FaceRecognizer::FaceRecognizer() {}

QVector<double> FaceRecognizer::computeLBPHistogram(const QImage& grayImage) {
    QVector<double> histogram(256, 0.0);
    int w = grayImage.width();
    int h = grayImage.height();

    if (w < 3 || h < 3) {
        return histogram;
    }

    // 8-neighbor offsets in clockwise order starting from top-left
    const int dx[8] = {-1,  0,  1, 1, 1, 0, -1, -1};
    const int dy[8] = {-1, -1, -1, 0, 1, 1,  1,  0};

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int center = qGray(grayImage.pixel(x, y));
            int code = 0;

            for (int i = 0; i < 8; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                int neighbor = qGray(grayImage.pixel(nx, ny));
                if (neighbor >= center) {
                    code |= (1 << i);
                }
            }

            histogram[code]++;
        }
    }

    // Normalize
    double sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += histogram[i];
    }
    if (sum > 0) {
        for (int i = 0; i < 256; i++) {
            histogram[i] /= sum;
        }
    }

    return histogram;
}

QVector<double> FaceRecognizer::extractFeatures(const QImage& faceImage) {
    if (faceImage.isNull()) {
        return QVector<double>();
    }

    // Convert to grayscale
    QImage gray = faceImage.convertToFormat(QImage::Format_Grayscale8);

    // Resize to standard size for consistency
    QImage resized = gray.scaled(100, 120, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    return computeLBPHistogram(resized);
}

double FaceRecognizer::compareFeatures(const QVector<double>& feat1, const QVector<double>& feat2) {
    if (feat1.size() != feat2.size() || feat1.isEmpty()) {
        return 1.0; // Maximum distance
    }

    double dist = 0.0;
    for (int i = 0; i < feat1.size(); i++) {
        double sum = feat1[i] + feat2[i];
        if (sum > 0) {
            double diff = feat1[i] - feat2[i];
            dist += (diff * diff) / sum;
        }
    }

    return dist / 2.0;
}

QByteArray FaceRecognizer::serializeFeatures(const QVector<double>& features) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << static_cast<quint32>(features.size());
    for (double val : features) {
        stream << val;
    }
    return data;
}

QVector<double> FaceRecognizer::deserializeFeatures(const QByteArray& data) {
    QVector<double> features;
    QDataStream stream(data);
    quint32 size;
    stream >> size;
    features.reserve(size);
    for (quint32 i = 0; i < size; i++) {
        double val;
        stream >> val;
        features.append(val);
    }
    return features;
}