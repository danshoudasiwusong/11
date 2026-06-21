#include "facedetector.h"
#include <QPainter>
#include <algorithm>

FaceDetector::FaceDetector() {}

bool FaceDetector::isSkinPixel(int r, int g, int b) {
    // YCbCr skin color detection
    // Convert RGB to YCbCr
    int y  = (77 * r + 150 * g +  29 * b) >> 8;
    int cb = (( -43 * r - 85 * g + 128 * b) >> 8) + 128;
    int cr = (( 128 * r - 107 * g - 21 * b) >> 8) + 128;

    // Skin thresholds
    if (y < 80) return false;
    if (cb < 77 || cb > 127) return false;
    if (cr < 133 || cr > 173) return false;

    // Additional RGB constraints
    if (r <= g || r <= b) return false;
    if (r - g < 15) return false;
    if (std::abs(r - g) < 10) return false;

    return true;
}

QImage FaceDetector::toGrayscale(const QImage& image) {
    return image.convertToFormat(QImage::Format_Grayscale8);
}

QImage FaceDetector::erode(const QImage& binary, int kernelSize) {
    int w = binary.width();
    int h = binary.height();
    int half = kernelSize / 2;
    QImage result(w, h, QImage::Format_Grayscale8);
    result.fill(0);

    for (int y = half; y < h - half; y++) {
        for (int x = half; x < w - half; x++) {
            bool allWhite = true;
            for (int ky = -half; ky <= half && allWhite; ky++) {
                for (int kx = -half; kx <= half && allWhite; kx++) {
                    if (qGray(binary.pixel(x + kx, y + ky)) == 0) {
                        allWhite = false;
                    }
                }
            }
            if (allWhite) {
                result.setPixel(x, y, qRgb(255, 255, 255));
            }
        }
    }
    return result;
}

QImage FaceDetector::dilate(const QImage& binary, int kernelSize) {
    int w = binary.width();
    int h = binary.height();
    int half = kernelSize / 2;
    QImage result(w, h, QImage::Format_Grayscale8);
    result.fill(0);

    for (int y = half; y < h - half; y++) {
        for (int x = half; x < w - half; x++) {
            bool anyWhite = false;
            for (int ky = -half; ky <= half && !anyWhite; ky++) {
                for (int kx = -half; kx <= half && !anyWhite; kx++) {
                    if (qGray(binary.pixel(x + kx, y + ky)) > 0) {
                        anyWhite = true;
                    }
                }
            }
            if (anyWhite) {
                result.setPixel(x, y, qRgb(255, 255, 255));
            }
        }
    }
    return result;
}

QRect FaceDetector::detectFace(const QImage& image) {
    if (image.isNull() || image.width() < 20 || image.height() < 20) {
        return QRect();
    }

    int w = image.width();
    int h = image.height();

    // Create binary skin mask
    QImage skinMask(w, h, QImage::Format_Grayscale8);
    skinMask.fill(0);

    int skinCount = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            QRgb pixel = image.pixel(x, y);
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);
            if (isSkinPixel(r, g, b)) {
                skinMask.setPixel(x, y, qRgb(255, 255, 255));
                skinCount++;
            }
        }
    }

    // Need minimum skin pixels
    if (skinCount < w * h * 0.02) {
        return QRect();
    }

    // Morphological operations to clean up
    QImage cleaned = erode(skinMask, 3);
    cleaned = dilate(cleaned, 5);
    cleaned = erode(cleaned, 3);

    // Find largest connected component (simple bounding box approach)
    // We'll scan for the largest cluster of white pixels
    int minX = w, minY = h, maxX = 0, maxY = 0;
    int whiteCount = 0;

    // Use a simple flood-fill based approach to find largest region
    // First pass: find bounds of all white pixels
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (qGray(cleaned.pixel(x, y)) > 128) {
                if (x < minX) minX = x;
                if (y < minY) minY = y;
                if (x > maxX) maxX = x;
                if (y > maxY) maxY = y;
                whiteCount++;
            }
        }
    }

    if (whiteCount < 100) {
        return QRect();
    }

    // Compute bounding box with some expansion
    int faceW = maxX - minX + 1;
    int faceH = maxY - minY + 1;

    // Aspect ratio check: face should be roughly oval/taller than wide
    double aspectRatio = static_cast<double>(faceH) / faceW;
    if (aspectRatio < 0.6 || aspectRatio > 2.0) {
        return QRect();
    }

    // Minimum face size
    if (faceW < 30 || faceH < 40) {
        return QRect();
    }

    // Expand rect by 15% to include forehead and chin
    int expandX = faceW * 0.15;
    int expandY = faceH * 0.15;
    minX = std::max(0, minX - expandX);
    minY = std::max(0, minY - static_cast<int>(expandY * 1.5)); // More expansion upward for forehead
    maxX = std::min(w - 1, maxX + expandX);
    maxY = std::min(h - 1, maxY + static_cast<int>(expandY * 0.8));

    return QRect(minX, minY, maxX - minX + 1, maxY - minY + 1);
}
