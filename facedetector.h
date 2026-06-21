#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <QImage>
#include <QRect>
#include <QPoint>
#include <QVector>

class FaceDetector {
public:
    FaceDetector();

    // Detect face in image, returns bounding rect. Returns invalid rect if no face found.
    QRect detectFace(const QImage& image);

private:
    bool isSkinPixel(int r, int g, int b);
    QImage toGrayscale(const QImage& image);
    QImage erode(const QImage& binary, int kernelSize = 3);
    QImage dilate(const QImage& binary, int kernelSize = 3);
};

#endif // FACEDETECTOR_H
