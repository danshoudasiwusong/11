#include "appcontroller.h"
#include "logindialog.h"
#include "mainwindow.h"
#include <QApplication>
#include <QTimer>

AppController::AppController(DatabaseManager* db, QObject* parent)
    : QObject(parent)
    , m_db(db)
    , m_mainWindow(nullptr)
{
}

AppController::~AppController() {
    if (m_mainWindow) {
        m_mainWindow->deleteLater();
        m_mainWindow = nullptr;
    }
}

void AppController::start() {
    showLogin();
}

void AppController::showLogin() {
    LoginDialog loginDialog(m_db);
    if (loginDialog.exec() == QDialog::Accepted) {
        QString username = loginDialog.loggedInUser();
        showMainWindow(username);
    } else {
        QApplication::quit();
    }
}

void AppController::showMainWindow(const QString& username) {
    m_mainWindow = new MainWindow(username, m_db);
    QObject::connect(m_mainWindow, &MainWindow::logoutRequested,
                     this, &AppController::onLogout);
    m_mainWindow->show();
}

void AppController::onLogout() {
    if (m_mainWindow) {
        m_mainWindow->hide();
        m_mainWindow->deleteLater();
        m_mainWindow = nullptr;
    }
    QTimer::singleShot(100, this, &AppController::showLogin);
}