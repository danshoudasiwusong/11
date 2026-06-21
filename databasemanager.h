#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QByteArray>
#include <QList>
#include <QPair>
#include <QSqlDatabase>

struct UserRecord {
    QString username;
    QString passwordHash;
    QByteArray faceData;
};

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool initialize();

    // User management
    bool addUser(const QString& username, const QString& password, const QByteArray& faceData);
    bool userExists(const QString& username);
    bool verifyPassword(const QString& username, const QString& password);

    // Face data
    QList<QPair<QString, QByteArray>> getAllFaceData();
    QByteArray getFaceData(const QString& username);

private:
    QString hashPassword(const QString& password);
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
