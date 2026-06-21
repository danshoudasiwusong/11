#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QFont>

MainWindow::MainWindow(const QString& username, DatabaseManager* db, QWidget* parent)
    : QMainWindow(parent)
    , m_username(username)
    , m_db(db)
{
    setupUi();
    setWindowTitle("FaceLogin - Dashboard");
    resize(600, 400);

    // Center on screen
    if (auto* screen = QApplication::primaryScreen()) {
        QRect screenGeometry = screen->availableGeometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* layout = new QVBoxLayout(centralWidget);
    layout->setSpacing(20);
    layout->setContentsMargins(40, 40, 40, 40);

    // Welcome
    m_welcomeLabel = new QLabel(QString("<h1>Welcome, %1!</h1>").arg(m_username));
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_welcomeLabel);

    // Info
    m_infoLabel = new QLabel(
        "You have successfully logged in using the Facial Recognition Login System.\n\n"
        "This system supports two authentication methods:\n"
        "• Password-based login\n"
        "• Face recognition login (LBP-based)\n\n"
        "Your face data and credentials are securely stored in a local SQLite database."
    );
    m_infoLabel->setAlignment(Qt::AlignCenter);
    m_infoLabel->setWordWrap(true);
    m_infoLabel->setStyleSheet("QLabel { font-size: 13px; color: #555; }");
    layout->addWidget(m_infoLabel);

    layout->addStretch();

    // Logout button
    m_logoutBtn = new QPushButton("Logout");
    m_logoutBtn->setMinimumHeight(40);
    m_logoutBtn->setMinimumWidth(150);
    m_logoutBtn->setStyleSheet(
        "QPushButton { background-color: #D13438; color: white; border-radius: 6px; font-size: 15px; }"
        "QPushButton:hover { background-color: #B82D31; }"
    );

    auto* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(m_logoutBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    connect(m_logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogout);
}

void MainWindow::onLogout() {
    emit logoutRequested();
    close();
}
