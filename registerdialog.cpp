#include "registerdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPainter>
#include <QMediaDevices>
#include <QMessageBox>

RegisterDialog::RegisterDialog(DatabaseManager* db, QWidget* parent)
    : QDialog(parent)
    , m_db(db)
    , m_camera(nullptr)
    , m_captureSession(nullptr)
    , m_videoSink(nullptr)
    , m_processTimer(nullptr)
    , m_faceDetected(false)
    , m_cameraRunning(false)
    , m_faceCaptured(false)
{
    setupUi();
    setWindowTitle("Register New Account");
    setMinimumSize(520, 550);
    onStartCamera();
}

RegisterDialog::~RegisterDialog() {
    onStopCamera();
}

void RegisterDialog::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    auto* titleLabel = new QLabel("<h3>Create New Account</h3>");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Form fields
    auto* formLayout = new QFormLayout();
    formLayout->setSpacing(8);

    m_usernameEdit = new QLineEdit();
    m_usernameEdit->setPlaceholderText("Choose a username");
    m_usernameEdit->setMinimumHeight(30);
    formLayout->addRow("Username:", m_usernameEdit);

    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setPlaceholderText("Choose a password (min 6 characters)");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumHeight(30);
    formLayout->addRow("Password:", m_passwordEdit);

    m_confirmPasswordEdit = new QLineEdit();
    m_confirmPasswordEdit->setPlaceholderText("Confirm password");
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit->setMinimumHeight(30);
    formLayout->addRow("Confirm:", m_confirmPasswordEdit);

    mainLayout->addLayout(formLayout);

    // Camera section
    auto* cameraGroup = new QVBoxLayout();
    m_cameraLabel = new QLabel();
    m_cameraLabel->setMinimumHeight(180);
    m_cameraLabel->setAlignment(Qt::AlignCenter);
    m_cameraLabel->setStyleSheet("QLabel { background-color: #2D2D2D; border: 2px solid #555; border-radius: 4px; }");
    m_cameraLabel->setText("Starting camera...");
    cameraGroup->addWidget(m_cameraLabel);

    auto* cameraBtnLayout = new QHBoxLayout();
    m_cameraToggleBtn = new QPushButton("Stop Camera");
    m_cameraToggleBtn->setMinimumHeight(32);
    m_cameraToggleBtn->setStyleSheet(
        "QPushButton { background-color: #555; color: white; border-radius: 4px; }"
        "QPushButton:hover { background-color: #666; }"
    );
    cameraBtnLayout->addWidget(m_cameraToggleBtn);

    m_captureBtn = new QPushButton("Capture Face");
    m_captureBtn->setMinimumHeight(32);
    m_captureBtn->setEnabled(false);
    m_captureBtn->setStyleSheet(
        "QPushButton { background-color: #0078D4; color: white; border-radius: 4px; }"
        "QPushButton:hover { background-color: #106EBE; }"
        "QPushButton:disabled { background-color: #555; color: #999; }"
    );
    cameraBtnLayout->addWidget(m_captureBtn);
    cameraGroup->addLayout(cameraBtnLayout);

    mainLayout->addLayout(cameraGroup);

    // Status
    m_statusLabel = new QLabel("Position your face in front of the camera and capture.");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setStyleSheet("QLabel { color: #888; font-size: 12px; }");
    mainLayout->addWidget(m_statusLabel);

    // Register button
    m_registerBtn = new QPushButton("Complete Registration");
    m_registerBtn->setMinimumHeight(38);
    m_registerBtn->setEnabled(false);
    m_registerBtn->setStyleSheet(
        "QPushButton { background-color: #107C10; color: white; border-radius: 4px; font-size: 14px; }"
        "QPushButton:hover { background-color: #0E6A0E; }"
        "QPushButton:disabled { background-color: #555; color: #999; }"
    );
    mainLayout->addWidget(m_registerBtn);

    // Cancel button
    auto* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setMinimumHeight(32);
    cancelBtn->setStyleSheet(
        "QPushButton { background-color: #E8E8E8; color: #333; border-radius: 4px; }"
        "QPushButton:hover { background-color: #D0D0D0; }"
    );
    mainLayout->addWidget(cancelBtn);

    // Connections
    connect(m_cameraToggleBtn, &QPushButton::clicked, this, [this]() {
        if (m_cameraRunning) {
            onStopCamera();
            m_cameraToggleBtn->setText("Start Camera");
        } else {
            onStartCamera();
            m_cameraToggleBtn->setText("Stop Camera");
        }
    });
    connect(m_captureBtn, &QPushButton::clicked, this, &RegisterDialog::onCaptureFace);
    connect(m_registerBtn, &QPushButton::clicked, this, &RegisterDialog::onRegister);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    m_processTimer = new QTimer(this);
    connect(m_processTimer, &QTimer::timeout, this, &RegisterDialog::onProcessFrame);
}

void RegisterDialog::onStartCamera() {
    if (m_cameraRunning) return;

    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        QMessageBox::warning(this, "No Camera", "No camera device found.");
        return;
    }

    m_camera = new QCamera(cameras.first(), this);
    m_captureSession = new QMediaCaptureSession(this);
    m_captureSession->setCamera(m_camera);

    m_videoSink = new QVideoSink(this);
    m_captureSession->setVideoSink(m_videoSink);

    connect(m_videoSink, &QVideoSink::videoFrameChanged,
            this, &RegisterDialog::onFrameCaptured);

    m_camera->start();
    m_cameraRunning = true;
    m_processTimer->start(300);
}

void RegisterDialog::onStopCamera() {
    m_processTimer->stop();

    if (m_camera) {
        m_camera->stop();
        delete m_camera;
        m_camera = nullptr;
    }

    if (m_videoSink) {
        delete m_videoSink;
        m_videoSink = nullptr;
    }

    if (m_captureSession) {
        delete m_captureSession;
        m_captureSession = nullptr;
    }

    m_cameraRunning = false;
}

void RegisterDialog::onFrameCaptured(const QVideoFrame& frame) {
    if (!frame.isValid()) return;

    QVideoFrame cloneFrame(frame);
    if (!cloneFrame.map(QVideoFrame::ReadOnly)) return;

    QImage image = cloneFrame.toImage();
    cloneFrame.unmap();

    if (image.isNull()) return;

    m_lastFrame = image.mirrored(true, false);
}

void RegisterDialog::onProcessFrame() {
    if (m_lastFrame.isNull() || m_faceCaptured) return;

    QImage displayImage = m_lastFrame.copy();
    QRect faceRect = m_faceDetector.detectFace(m_lastFrame);

    if (faceRect.isValid()) {
        m_lastFaceRect = faceRect;
        m_faceDetected = true;
        m_captureBtn->setEnabled(true);
        drawFaceRect(displayImage, faceRect);
    } else {
        m_faceDetected = false;
        m_captureBtn->setEnabled(false);
    }

    QPixmap pixmap = QPixmap::fromImage(displayImage).scaled(
        m_cameraLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation
    );
    m_cameraLabel->setPixmap(pixmap);

    if (m_faceDetected && !m_faceCaptured) {
        m_statusLabel->setText("Face detected! Click 'Capture Face' to save.");
        m_statusLabel->setStyleSheet("QLabel { color: #00AA00; font-size: 12px; }");
    }
}

void RegisterDialog::drawFaceRect(QImage& image, const QRect& rect) {
    QPainter painter(&image);
    painter.setPen(QPen(QColor(0, 255, 0), 2));
    painter.drawRect(rect);
    painter.end();
}

void RegisterDialog::onCaptureFace() {
    if (!m_faceDetected || m_lastFaceRect.isNull()) {
        QMessageBox::warning(this, "Capture Failed",
            "No face detected. Please adjust your position.");
        return;
    }

    m_capturedFace = m_lastFrame.copy(m_lastFaceRect);
    m_faceCaptured = true;
    m_captureBtn->setEnabled(false);
    m_registerBtn->setEnabled(true);

    // Show captured face
    QImage displayImage = m_lastFrame.copy();
    QPainter painter(&displayImage);
    painter.setPen(QPen(QColor(0, 200, 0), 2));
    painter.drawRect(m_lastFaceRect);
    painter.end();

    QPixmap pixmap = QPixmap::fromImage(displayImage).scaled(
        m_cameraLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation
    );
    m_cameraLabel->setPixmap(pixmap);

    m_statusLabel->setText(
        "Face captured! Fill in your credentials and click 'Complete Registration'.");
    m_statusLabel->setStyleSheet("QLabel { color: #00CC00; font-size: 12px; }");
}

void RegisterDialog::onRegister() {
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    QString confirm = m_confirmPasswordEdit->text();

    // Validation
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a username.");
        m_usernameEdit->setFocus();
        return;
    }

    if (username.length() < 3) {
        QMessageBox::warning(this, "Validation Error",
            "Username must be at least 3 characters.");
        m_usernameEdit->setFocus();
        return;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a password.");
        m_passwordEdit->setFocus();
        return;
    }

    if (password.length() < 6) {
        QMessageBox::warning(this, "Validation Error",
            "Password must be at least 6 characters.");
        m_passwordEdit->setFocus();
        return;
    }

    if (password != confirm) {
        QMessageBox::warning(this, "Validation Error", "Passwords do not match.");
        m_confirmPasswordEdit->clear();
        m_confirmPasswordEdit->setFocus();
        return;
    }

    if (m_db->userExists(username)) {
        QMessageBox::warning(this, "Validation Error",
            "Username already exists. Please choose another.");
        m_usernameEdit->setFocus();
        return;
    }

    if (!m_faceCaptured || m_capturedFace.isNull()) {
        QMessageBox::warning(this, "Validation Error",
            "Please capture your face first.");
        return;
    }

    // Extract face features
    FaceRecognizer recognizer;
    QVector<double> features = recognizer.extractFeatures(m_capturedFace);
    if (features.isEmpty()) {
        QMessageBox::warning(this, "Error",
            "Failed to extract face features. Please try capturing again.");
        return;
    }

    QByteArray faceData = recognizer.serializeFeatures(features);

    // Save to database
    if (m_db->addUser(username, password, faceData)) {
        QMessageBox::information(this, "Success",
            QString("Account '%1' created successfully!\n"
                    "You can now login with password or face recognition.")
                .arg(username));
        accept();
    } else {
        QMessageBox::critical(this, "Error",
            "Failed to create account. Please try again.");
    }
}