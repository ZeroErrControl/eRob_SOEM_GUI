#pragma once

#include <QObject>
#include <QtNetwork/QNetworkInterface>
#include "component_manager.h"
#include "monitor_window.h"

class MonitorWindow;

class NetworkManager : public QObject {
    Q_OBJECT
public:
    NetworkManager(MonitorWindow* parent, ComponentManager::NetworkComponents* networkComps);
    
    // 网络管理方法
    void refreshNetworkInterfaces();
    void confirmNetworkInterface(const QString& interfaceName);
    void updateNetworkStatusIndicator(bool connected);
    
public slots:
    void onNetworkChanged(int index);
    void onRefreshNetwork();
    void onConfirmNetwork();
    
private:
    MonitorWindow* mainWindow;
    monitor::SharedData& sharedData;
    ComponentManager::NetworkComponents* networkComps;
}; 