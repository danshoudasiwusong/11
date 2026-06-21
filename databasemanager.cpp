#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

DatabaseManager::DatabaseManager() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::initialize() {
    QString dbPath = QCoreApplication::applicationDirPath() + "/users.db";
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qWarning() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);
    bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT UNIQUE NOT NULL,"
        "  password_hash TEXT NOT NULL,"
        "  face_data BLOB"
        ")"
    );

    if (!ok) {
        qWarning() << "Failed to create table:" << query.lastError().text();
        return false;
    }

    return true;
}

QString DatabaseManager::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
    );
    return QString::fromLatin1(hash.toHex());
}

bool DatabaseManager::addUser(const QString& username, const QString& password, const QByteArray& faceData) {
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO users (username, password_hash, face_data) VALUES (?, ?, ?)"
    );
    query.addBindValue(username);
    query.addBindValue(hashPassword(password));
    query.addBindValue(faceData);

    if (!query.exec()) {
        qWarning() << "Failed to add user:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::userExists(const QString& username) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    query.addBindValue(username);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::verifyPassword(const QString& username, const QString& password) {
    QSqlQuery query(m_db);
    query.prepare("SELECT password_hash FROM users WHERE username = ?");
    query.addBindValue(username);

    if (query.exec() && query.next()) {
        QString storedHash = query.value(0).toString();
        return storedHash == hashPassword(password);
    }
    return false;
}

QList<QPair<QString, QByteArray>> DatabaseManager::getAllFaceData() {
    QList<QPair<QString, QByteArray>> result;
    QSqlQuery query(m_db);
    query.exec("SELECT username, face_data FROM users WHERE face_data IS NOT NULL AND face_data != ''");

    while (query.next()) {
        QString username = query.value(0).toString();
        QByteArray faceData = query.value(1).toByteArray();
        result.append(qMakePair(username, faceData));
    }

    return result;
}

QByteArray DatabaseManager::getFaceData(const QString& username) {
    QSqlQuery query(m_db);
    query.prepare("SELECT face_data FROM users WHERE username = ?");
    query.addBindValue(username);

    if (query.exec() && query.next()) {
        return query.value(0).toByteArray();
    }
    return QByteArray();
}
