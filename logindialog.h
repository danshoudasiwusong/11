#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QVideoFrame>
#include <QTimer>
#include "facedetector.h"
#include "facerecognizer.h"
#include "databasemanager.h"

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(DatabaseManager* db, QWidget* parent = nullptr);
    ~LoginDialog();
    QString loggedInUser() const { return m_loggedInUser; }

private slots:
    void onPasswordLogin();
    void onStartCamera();
    void onStopCamera();
    void onRecognizeFace();
    void onFrameCaptured(const QVideoFrame& frame);
    void onOpenRegister();
    void onProcessFrame();

private:
    void setupUi();
    void drawFaceRect(QImage& image, const QRect& rect);

    DatabaseManager* m_db;
    QString m_loggedInUser;

    // UI elements
    QTabWidget* m_tabWidget;
    // Password tab
    QLineEdit* m_usernameEdit;
    QLineEdit* m_passwordEdit;
    QPushButton* m_passwordLoginBtn;
    // Face tab
    QLabel* m_cameraLabel;
    QPushButton* m_cameraToggleBtn;
    QPushButton* m_recognizeBtn;
    // Common
    QPushButton* m_registerBtn;
    QLabel* m_statusLabel;

    // Camera
    QCamera* m_camera;
    QMediaCaptureSession* m_captureSession;
    QVideoSink* m_videoSink;
    QTimer* m_processTimer;

    // Face processing
    FaceDetector m_faceDetector;
    FaceRecognizer m_faceRecognizer;
    QImage m_lastFrame;
    QRect m_lastFaceRect;
    bool m_faceDetected;
    bool m_cameraRunning;
};

#endif // LOGINDIALOG_H
