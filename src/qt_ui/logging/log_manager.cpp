#include "logging/log_manager.h"
#include "monitor_window.h"

LogManager::LogManager(MonitorWindow* parent, QTextEdit* logDisplay)
    : QObject(parent), mainWindow(parent), logDisplay(logDisplay), minLogLevel(LogLevel::INFO) {
    
    // Initialize log display
    logDisplay->setReadOnly(true);
    logDisplay->document()->setDocumentMargin(5);
    
    // Add welcome message
    appendLog("System log initialized", LogLevel::INFO);
    appendLog("Welcome to the EtherCAT Servo Motor Control and Monitoring System", LogLevel::SUCCESS);
}

void LogManager::appendLog(const QString& message, LogLevel level) {
    // Do not display if below minimum log level
    if (level < minLogLevel) return;
    
    QDateTime now = QDateTime::currentDateTime();
    QString timeStamp = now.toString("hh:mm:ss.zzz");
    
    QString colorCode;
    QString prefix;
    
    switch (level) {
        case LogLevel::INFO:
            colorCode = "black";
            prefix = "[INFO]";
            break;
        case LogLevel::WARNING:
            colorCode = "orange";
            prefix = "[WARNING]";
            break;
        case LogLevel::ERROR:
            colorCode = "red";
            prefix = "[ERROR]";
            break;
        case LogLevel::SUCCESS:
            colorCode = "green";
            prefix = "[SUCCESS]";
            break;
    }
    
    logDisplay->append(QString("<span style='color:%1'>[%2] %3 %4</span>")
                      .arg(colorCode)
                      .arg(timeStamp)
                      .arg(prefix)
                      .arg(message));
    logDisplay->moveCursor(QTextCursor::End);
}

void LogManager::clearLog() {
    logDisplay->clear();
    appendLog("Log cleared", LogLevel::INFO);
}

void LogManager::setLogFilter(LogLevel minLevel) {
    minLogLevel = minLevel;
    appendLog(QString("Log filter level set to: %1").arg(static_cast<int>(minLevel)), LogLevel::INFO);
}