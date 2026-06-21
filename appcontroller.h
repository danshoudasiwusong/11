#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QString>
#include "databasemanager.h"

class MainWindow;
class LoginDialog;

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(DatabaseManager* db, QObject* parent = nullptr);
    ~AppController();

public slots:
    void start();

private slots:
    void showLogin();
    void onLogout();

private:
    void showMainWindow(const QString& username);

    DatabaseManager* m_db;
    MainWindow* m_mainWindow;
};

#endif // APPCONTROLLER_H