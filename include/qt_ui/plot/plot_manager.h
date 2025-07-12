#pragma once

#include <QObject>
#include "qcustomplot.h"
#include "component_manager.h"
#include "monitor_window.h"

// 前向声明
class MonitorWindow;

class PlotManager : public QObject {
    Q_OBJECT
public:
    PlotManager(MonitorWindow* parent, ComponentManager::PlotComponents* plotComps);
    
    // 图表初始化
    void initializePlot();
    
    // 图表控制方法
    void updateYAxisRange(bool forceUpdate = false);
    void setupInteractions();
    
    // 数据更新方法
    void updatePlotData(double key, const monitor::SharedData::DataPoint& point);
    void cleanupOldData(double key);
    void replotWithCurrentSettings(double key);
    
private:
    MonitorWindow* mainWindow;
    ComponentManager::PlotComponents* plotComps;
    bool lockView;
    bool autoYRange;
    
    // 鼠标交互处理
    void handleMouseInteractions();
}; 