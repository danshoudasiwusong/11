#include "logindialog.h"
#include "registerdialog.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPainter>
#include <QMediaDevices>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>

LoginDialog::LoginDialog(DatabaseManager* db, QWidget* parent)
    : QDialog(parent)
    , m_db(db)
    , m_camera(nullptr)
    , m_captureSession(nullptr)
    , m_videoSink(nullptr)
    , m_processTimer(nullptr)
    , m_faceDetected(false)
    , m_cameraRunning(false)
{
    setupUi();
    setWindowTitle("Facial Recognition Login System");
    setMinimumSize(550, 480);
}

LoginDialog::~LoginDialog() {
    onStopCamera();
}

void LoginDialog::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    auto* titleLabel = new QLabel("<h2>Welcome to FaceLogin</h2>");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Tab widget
    m_tabWidget = new QTabWidget();

    // --- Password Login Tab ---
    auto* passwordTab = new QWidget();
    auto* passwordLayout = new QVBoxLayout(passwordTab);
    passwordLayout->setSpacing(10);

    auto* formLayout = new QFormLayout();
    m_usernameEdit = new QLineEdit();
    m_usernameEdit->setPlaceholderText("Enter username");
    m_usernameEdit->setMinimumHeight(32);
    formLayout->addRow("Username:", m_usernameEdit);

    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setPlaceholderText("Enter password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumHeight(32);
    formLayout->addRow("Password:", m_passwordEdit);

    passwordLayout->addLayout(formLayout);

    m_passwordLoginBtn = new QPushButton("Login");
    m_passwordLoginBtn->setMinimumHeight(36);
    m_passwordLoginBtn->setStyleSheet(
        "QPushButton { background-color: #0078D4; color: white; border-radius: 4px; font-size: 14px; }"
        "QPushButton:hover { background-color: #106EBE; }"
    );
    passwordLayout->addWidget(m_passwordLoginBtn);
    passwordLayout->addStretch();

    m_tabWidget->addTab(passwordTab, "Password Login");

    // --- Face Recognition Tab ---
    auto* faceTab = new QWidget();
    auto* faceLayout = new QVBoxLayout(faceTab);
    faceLayout->setSpacing(10);

    m_cameraLabel = new QLabel();
    m_cameraLabel->setMinimumHeight(200);
    m_cameraLabel->setAlignment(Qt::AlignCenter);
    m_cameraLabel->setStyleSheet("QLabel { background-color: #2D2D2D; border: 2px solid #555; border-radius: 4px; }");
    m_cameraLabel->setText("Camera feed will appear here");
    faceLayout->addWidget(m_cameraLabel);

    auto* faceBtnLayout = new QHBoxLayout();
    m_cameraToggleBtn = new QPushButton("Start Camera");
    m_cameraToggleBtn->setMinimumHeight(36);
    m_cameraToggleBtn->setStyleSheet(
        "QPushButton { background-color: #0078D4; color: white; border-radius: 4px; font-size: 14px; }"
        "QPushButton:hover { background-color: #106EBE; }"
    );
    faceBtnLayout->addWidget(m_cameraToggleBtn);

    m_recognizeBtn = new QPushButton("Recognize Face");
    m_recognizeBtn->setMinimumHeight(36);
    m_recognizeBtn->setEnabled(false);
    m_recognizeBtn->setStyleSheet(
        "QPushButton { background-color: #107C10; color: white; border-radius: 4px; font-size: 14px; }"
        "QPushButton:hover { background-color: #0E6A0E; }"
        "QPushButton:disabled { background-color: #555; color: #999; }"
    );
    faceBtnLayout->addWidget(m_recognizeBtn);

    faceLayout->addLayout(faceBtnLayout);
    faceLayout->addStretch();

    m_tabWidget->addTab(faceTab, "Face Recognition");

    mainLayout->addWidget(m_tabWidget);

    // Status label
    m_statusLabel = new QLabel();
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("QLabel { color: #888; font-size: 12px; }");
    mainLayout->addWidget(m_statusLabel);

    // Register button
    m_registerBtn = new QPushButton("Register New Account");
    m_registerBtn->setMinimumHeight(36);
    m_registerBtn->setStyleSheet(
        "QPushButton { background-color: #E8E8E8; color: #333; border-radius: 4px; font-size: 13px; }"
        "QPushButton:hover { background-color: #D0D0D0; }"
    );
    mainLayout->addWidget(m_registerBtn);

    // Connections
    connect(m_passwordLoginBtn, &QPushButton::clicked, this, &LoginDialog::onPasswordLogin);
    connect(m_cameraToggleBtn, &QPushButton::clicked, this, [this]() {
        if (m_cameraRunning) {
            onStopCamera();
        } else {
            onStartCamera();
        }
    });
    connect(m_recognizeBtn, &QPushButton::clicked, this, &LoginDialog::onRecognizeFace);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginDialog::onOpenRegister);

    // Enter key triggers login on password tab
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onPasswordLogin);

    // Process timer for face detection
    m_processTimer = new QTimer(this);
    connect(m_processTimer, &QTimer::timeout, this, &LoginDialog::onProcessFrame);
}

void LoginDialog::onStartCamera() {
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

    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &LoginDialog::onFrameCaptured);

    m_camera->start();
    m_cameraRunning = true;
    m_processTimer->start(300); // Process frame every 300ms

    m_cameraToggleBtn->setText("Stop Camera");
    m_cameraLabel->setText("Starting camera...");
    m_statusLabel->setText("Camera started. Position your face in front of the camera.");
    m_statusLabel->setStyleSheet("QLabel { color: #00AA00; font-size: 12px; }");
}

void LoginDialog::onStopCamera() {
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
    m_faceDetected = false;
    m_recognizeBtn->setEnabled(false);

    m_cameraToggleBtn->setText("Start Camera");
    m_cameraLabel->setText("Camera feed will appear here");
    m_cameraLabel->setStyleSheet("QLabel { background-color: #2D2D2D; border: 2px solid #555; border-radius: 4px; }");
    m_statusLabel->setText("");
    m_statusLabel->setStyleSheet("QLabel { color: #888; font-size: 12px; }");
}

void LoginDialog::onFrameCaptured(const QVideoFrame& frame) {
    if (!frame.isValid()) return;

    QVideoFrame cloneFrame(frame);
    if (!cloneFrame.map(QVideoFrame::ReadOnly)) return;

    QImage image = cloneFrame.toImage();
    cloneFrame.unmap();

    if (image.isNull()) return;

    // Mirror the image horizontally for natural selfie view
    m_lastFrame = image.mirrored(true, false);
}

void LoginDialog::onProcessFrame() {
    if (m_lastFrame.isNull()) return;

    QImage displayImage = m_lastFrame.copy();
    QRect faceRect = m_faceDetector.detectFace(m_lastFrame);

    if (faceRect.isValid()) {
        m_lastFaceRect = faceRect;
        m_faceDetected = true;
        m_recognizeBtn->setEnabled(true);
        drawFaceRect(displayImage, faceRect);
    } else {
        m_faceDetected = false;
        m_recognizeBtn->setEnabled(false);
    }

    // Scale for display
    QPixmap pixmap = QPixmap::fromImage(displayImage).scaled(
        m_cameraLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation
    );
    m_cameraLabel->setPixmap(pixmap);

    if (m_faceDetected) {
        m_statusLabel->setText("Face detected! Click 'Recognize Face' to login.");
        m_statusLabel->setStyleSheet("QLabel { color: #00AA00; font-size: 12px; }");
    } else {
        m_statusLabel->setText("No face detected. Please adjust your position.");
        m_statusLabel->setStyleSheet("QLabel { color: #FFAA00; font-size: 12px; }");
    }
}

void LoginDialog::drawFaceRect(QImage& image, const QRect& rect) {
    QPainter painter(&image);
    painter.setPen(QPen(QColor(0, 255, 0), 2));
    painter.drawRect(rect);
    painter.end();
}

void LoginDialog::onRecognizeFace() {
    if (!m_faceDetected || m_lastFaceRect.isNull()) {
        QMessageBox::warning(this, "Recognition Failed", "No face detected. Please try again.");
        return;
    }

    // Extract face ROI
    QImage faceROI = m_lastFrame.copy(m_lastFaceRect);

    // Extract features
    QVector<double> currentFeatures = m_faceRecognizer.extractFeatures(faceROI);
    if (currentFeatures.isEmpty()) {
        QMessageBox::warning(this, "Error", "Failed to extract face features.");
        return;
    }

    // Compare with all stored faces
    auto allFaces = m_db->getAllFaceData();
    if (allFaces.isEmpty()) {
        QMessageBox::information(this, "No Users", "No registered users with face data found. Please register first.");
        return;
    }

    QString bestMatch;
    double bestDistance = 1.0;

    for (const auto& pair : allFaces) {
        const QString& username = pair.first;
        const QByteArray& storedData = pair.second;

        QVector<double> storedFeatures = m_faceRecognizer.deserializeFeatures(storedData);
        if (storedFeatures.isEmpty()) continue;

        double distance = m_faceRecognizer.compareFeatures(currentFeatures, storedFeatures);
        if (distance < bestDistance) {
            bestDistance = distance;
            bestMatch = username;
        }
    }

    if (bestDistance < FaceRecognizer::RECOGNITION_THRESHOLD && !bestMatch.isEmpty()) {
        m_loggedInUser = bestMatch;
        m_statusLabel->setText(QString("Welcome, %1! Login successful.").arg(bestMatch));
        m_statusLabel->setStyleSheet("QLabel { color: #00AA00; font-size: 12px; }");
        accept();
    } else {
        m_statusLabel->setText(QString("Face not recognized. Best match distance: %1").arg(bestDistance, 0, 'f', 3));
        m_statusLabel->setStyleSheet("QLabel { color: #FF4444; font-size: 12px; }");
        QMessageBox::warning(this, "Recognition Failed",
            "Face not recognized. Please try again or use password login.");
    }
}

void LoginDialog::onPasswordLogin() {
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter both username and password.");
        return;
    }

    if (!m_db->userExists(username)) {
        QMessageBox::warning(this, "Login Failed", "User does not exist.");
        return;
    }

    if (m_db->verifyPassword(username, password)) {
        m_loggedInUser = username;
        m_statusLabel->setText("Login successful!");
        m_statusLabel->setStyleSheet("QLabel { color: #00AA00; font-size: 12px; }");
        accept();
    } else {
        QMessageBox::warning(this, "Login Failed", "Incorrect password.");
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
    }
}

void LoginDialog::onOpenRegister() {
    onStopCamera();
    RegisterDialog regDialog(m_db, this);
    if (regDialog.exec() == QDialog::Accepted) {
        m_statusLabel->setText("Registration successful! You can now login.");
        m_statusLabel->setStyleSheet("QLabel { color: #00AA00; font-size: 12px; }");
    }
}
