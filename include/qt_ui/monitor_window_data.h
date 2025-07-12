#pragma once

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include "monitor_window.h"
#include "component_manager.h"

// 前向声明
class QCustomPlot;

class MonitorWindowData : public QObject {
    Q_OBJECT
    
public:
    explicit MonitorWindowData(QObject* parent,
                              monitor::SharedData& sharedData,
                              ComponentManager::PlotComponents* plotComps,
                              QDateTime startTime);

    void startDataTimer();
    void stopDataTimer();
    
    // 设置视图控制状态
    void setLockView(bool locked);
    void setAutoYRange(bool autoRange);

public slots:
    void updatePlot();

private:
    monitor::SharedData& sharedData;
    ComponentManager::PlotComponents* plotComps;
    QTimer* dataTimer;
    QDateTime startTime;
    
    // 图表更新相关
    static const int MAX_POINTS = 5000;
    static const int UPDATE_INTERVAL = 10; // 10ms = 100Hz
    static const int CLEANUP_INTERVAL = 20; // 每20次更新清理一次
    static const int REPLOT_INTERVAL = 3;   // 每3次更新重绘一次
    
    // 视图控制
    bool lockView = false;
    bool autoYRange = true;
    
    // 辅助方法
    void cleanupOldData(double currentTime);
    void updateYAxisRange(bool forceUpdate = false);
    void updateXAxisRange(double currentTime);
}; 