#pragma once

#include <QObject>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QDateTime>
#include "component_manager.h"
#include "monitor_window.h"

// 前向声明
class QCustomPlot;
class QMouseEvent;

class MonitorWindowEvents : public QObject {
    Q_OBJECT
    
public:
    explicit MonitorWindowEvents(QMainWindow* window, 
                               monitor::SharedData& sharedData,
                               ComponentManager::NetworkComponents* networkComps,
                               ComponentManager::ModeComponents* modeComps,
                               ComponentManager::SpeedComponents* speedComps,
                               ComponentManager::ControlComponents* controlComps,
                               ComponentManager::PlotComponents* plotComps,
                               ComponentManager::TargetComponents* targetComps,
                               ComponentManager::PPComponents* ppComps,
                               ComponentManager::PVComponents* pvComps,
                               ComponentManager::PTComponents* ptComps,
                               ComponentManager::CSTComponents* cstComps,
                               ComponentManager::CSVComponents* csvComps,
                               ComponentManager::CSPComponents* cspComps,
                               QTextEdit* logDisplay,
                               QCheckBox* lockViewCheckBox,
                               QCheckBox* autoYRangeCheckBox,
                               QPushButton* resetViewBtn,
                               QDateTime startTime);

    void connectAllSignals();
    void setupMouseEvents();

public slots:
    // 图表显示控制
    void onVelocityToggled(bool checked);
    void onPositionToggled(bool checked);
    void onTorqueToggled(bool checked);
    void onModeToggled(bool checked);
    
    // 控制相关
    void onSpeedInputChanged();
    void onEnableButtonClicked();
    void onTargetValueChanged();
    
    // 模式相关
    void onModeChanged(int index);
    void onModeConfirmClicked();
    void onPPParamsConfirmed();
    void onPVParamsConfirmed();
    void onPTParamsConfirmed();
    void onCSTParamsConfirmed();
    void onCSPParamsConfirmed();
    
    // 网络相关
    void onNetworkChanged(int index);
    void onRefreshNetwork();
    void onConfirmNetwork();
    
    // 视图控制
    void onLockViewToggled(bool checked);
    void onAutoYRangeToggled(bool checked);
    void onResetViewClicked();
    void onMouseWheel();
    void onMouseMove(QMouseEvent* event);
    void onMouseDoubleClick();

private:
    QMainWindow* window;
    monitor::SharedData& sharedData;
    
    // 组件指针
    ComponentManager::NetworkComponents* networkComps;
    ComponentManager::ModeComponents* modeComps;
    ComponentManager::SpeedComponents* speedComps;
    ComponentManager::ControlComponents* controlComps;
    ComponentManager::PlotComponents* plotComps;
    ComponentManager::TargetComponents* targetComps;
    ComponentManager::PPComponents* ppComps;
    ComponentManager::PVComponents* pvComps;
    ComponentManager::PTComponents* ptComps;
    ComponentManager::CSTComponents* cstComps;
    ComponentManager::CSVComponents* csvComps;
    ComponentManager::CSPComponents* cspComps;
    
    // UI元素
    QTextEdit* logDisplay;
    QCheckBox* lockViewCheckBox;
    QCheckBox* autoYRangeCheckBox;
    QPushButton* resetViewBtn;
    QDateTime startTime;
    
    // 状态变量
    bool showVelocity = true;
    bool showPosition = false;
    bool showTorque = false;
    bool showMode = true;
    bool lockView = false;
    bool autoYRange = true;
    
    // 辅助方法
    void updateYAxisRange(bool forceUpdate = false);
    void appendLog(const QString& message, LogLevel level = LogLevel::INFO);
    void updateModePanel(int mode);
    void updateStatusBar(const QString& message, int timeout = 2000);
}; 