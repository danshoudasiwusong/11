#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QVideoFrame>
#include <QTimer>
#include "facedetector.h"
#include "facerecognizer.h"
#include "databasemanager.h"

class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    explicit RegisterDialog(DatabaseManager* db, QWidget* parent = nullptr);
    ~RegisterDialog();

private slots:
    void onStartCamera();
    void onStopCamera();
    void onCaptureFace();
    void onRegister();
    void onFrameCaptured(const QVideoFrame& frame);
    void onProcessFrame();

private:
    void setupUi();
    void drawFaceRect(QImage& image, const QRect& rect);

    DatabaseManager* m_db;

    QLineEdit* m_usernameEdit;
    QLineEdit* m_passwordEdit;
    QLineEdit* m_confirmPasswordEdit;
    QLabel* m_cameraLabel;
    QPushButton* m_cameraToggleBtn;
    QPushButton* m_captureBtn;
    QPushButton* m_registerBtn;
    QLabel* m_statusLabel;

    // Camera
    QCamera* m_camera;
    QMediaCaptureSession* m_captureSession;
    QVideoSink* m_videoSink;
    QTimer* m_processTimer;

    FaceDetector m_faceDetector;
    QImage m_lastFrame;
    QRect m_lastFaceRect;
    QImage m_capturedFace;
    bool m_faceDetected;
    bool m_cameraRunning;
    bool m_faceCaptured;
};

#endif // REGISTERDIALOG_H
