#pragma once

#include <QObject>
#include "component_manager.h"
#include "monitor_window.h"
#include "pdo_manager.h"

class MonitorWindow;

class MotorController : public QObject {
    Q_OBJECT
public:
    MotorController(MonitorWindow* parent, monitor::SharedData& sharedData, 
                    ComponentManager::ControlComponents* controlComps,
                    ComponentManager::TargetComponents* targetComps);
    
    // 电机控制方法
    void enableMotor();
    void disableMotor();
    void updateTargetValue(int value);
    void updateEnableButtonState();
    
    // 位置状态更新
    void updatePositionStatus(int32_t currentPosition, int32_t targetPosition);
    
public slots:
    void onEnableButtonClicked();
    void onTargetValueChanged();
    
private:
    MonitorWindow* mainWindow;
    monitor::SharedData& sharedData;
    ComponentManager::ControlComponents* controlComps;
    ComponentManager::TargetComponents* targetComps;
}; 