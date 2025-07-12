#pragma once

#include <QObject>
#include <QTextEdit>
#include <QDateTime>
#include "monitor_window.h"

class LogManager : public QObject {
    Q_OBJECT
public:
    LogManager(MonitorWindow* parent, QTextEdit* logDisplay);
    
    // 日志管理方法
    void appendLog(const QString& message, LogLevel level = LogLevel::INFO);
    void clearLog();
    void setLogFilter(LogLevel minLevel);
    
private:
    MonitorWindow* mainWindow;
    QTextEdit* logDisplay;
    LogLevel minLogLevel;
}; 