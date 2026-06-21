#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include <QTimer>
#include "appcontroller.h"
#include "databasemanager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set modern Fusion style
    app.setStyle(QStyleFactory::create("Fusion"));

    // Dark theme palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::WindowText, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Base, QColor(30, 30, 34));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::ToolTipText, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Text, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Button, QColor(55, 55, 58));
    darkPalette.setColor(QPalette::ButtonText, QColor(220, 220, 220));
    darkPalette.setColor(QPalette::Highlight, QColor(0, 120, 212));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(128, 128, 128));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(128, 128, 128));
    app.setPalette(darkPalette);

    // Fine-tuned stylesheet
    app.setStyleSheet(
        "QToolTip { color: #DDD; background-color: #333; border: 1px solid #555; }"
        "QLineEdit { padding: 4px 8px; border: 1px solid #555; border-radius: 3px; background-color: #3C3C3C; }"
        "QLineEdit:focus { border: 1px solid #0078D4; }"
        "QTabWidget::pane { border: 1px solid #444; }"
        "QTabBar::tab { background: #3C3C3C; padding: 8px 20px; border: 1px solid #444; }"
        "QTabBar::tab:selected { background: #505050; border-bottom: 2px solid #0078D4; }"
    );

    // Initialize database
    DatabaseManager db;
    if (!db.initialize()) {
        qCritical("Failed to initialize database!");
        return 1;
    }

    // Start the login flow via controller
    AppController controller(&db);
    QTimer::singleShot(0, &controller, &AppController::start);

    return app.exec();
}