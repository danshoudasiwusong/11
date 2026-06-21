#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "databasemanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString& username, DatabaseManager* db, QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void logoutRequested();

private slots:
    void onLogout();

private:
    void setupUi();

    QString m_username;
    DatabaseManager* m_db;
    QLabel* m_welcomeLabel;
    QLabel* m_infoLabel;
    QPushButton* m_logoutBtn;
};

#endif // MAINWINDOW_H
