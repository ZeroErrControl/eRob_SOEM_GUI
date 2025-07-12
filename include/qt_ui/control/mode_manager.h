#pragma once

#include <QObject>
#include "component_manager.h"
#include "monitor_window.h"
#include "sdo_manager.h"

class MonitorWindow;

class ModeManager : public QObject {
    Q_OBJECT
public:
    ModeManager(MonitorWindow* parent, monitor::SharedData& sharedData, 
               ComponentManager::ModeComponents* modeComps,
               ComponentManager::PPComponents* ppComps,
               ComponentManager::PVComponents* pvComps,
               ComponentManager::PTComponents* ptComps,
               ComponentManager::CSTComponents* cstComps,
               ComponentManager::CSVComponents* csvComps,
               ComponentManager::CSPComponents* cspComps,
               ComponentManager::TargetComponents* targetComps);
    
    // 模式管理方法
    void updateModePanel(int mode);
    void switchToMode(int mode);
    void confirmMode();
    
    // PP模式相关方法
    void confirmPPParams();
    
    // PV模式相关方法
    void confirmPVParams();
    
    // PT模式相关方法
    void confirmPTParams();
    
    // CST模式相关方法
    void confirmCSTParams();
    
    // 更新模式状态指示
    void updateModeStatusIndicator();
    
public slots:
    void onModeChanged(int index);
    void onModeConfirmClicked();
    void onPPParamsConfirmed();
    void onPVParamsConfirmed();
    void onPTParamsConfirmed();
    void onCSTParamsConfirmed();
    
private:
    MonitorWindow* mainWindow;
    monitor::SharedData& sharedData;
    ComponentManager::ModeComponents* modeComps;
    ComponentManager::PPComponents* ppComps;
    ComponentManager::PVComponents* pvComps;
    ComponentManager::PTComponents* ptComps;
    ComponentManager::CSTComponents* cstComps;
    ComponentManager::CSVComponents* csvComps;
    ComponentManager::CSPComponents* cspComps;
    ComponentManager::TargetComponents* targetComps;
}; 