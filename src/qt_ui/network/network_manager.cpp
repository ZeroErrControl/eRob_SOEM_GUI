#include "qt_ui/network/network_manager.h"
#include "monitor_window.h"

NetworkManager::NetworkManager(MonitorWindow* parent, ComponentManager::NetworkComponents* networkComps)
    : QObject(parent), mainWindow(parent), 
      sharedData(parent->getSharedData()),
      networkComps(networkComps) {
    
    // Don't call refreshNetworkInterfaces() in constructor
}

void NetworkManager::refreshNetworkInterfaces() {
    mainWindow->appendLog("Refreshing network interface list...", LogLevel::INFO);
    networkComps->selector->clear();
    
    // Get all network interfaces in the system
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    bool found = false;
    int interfaceCount = 0;
    
    for (const QNetworkInterface &interface : interfaces) {
        // Only add active Ethernet interfaces
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            
            QString name = interface.name();
            QString description = interface.humanReadableName();
            QString displayText = QString("%1 (%2)").arg(name).arg(description);
            
            networkComps->selector->addItem(displayText, name);
            found = true;
            interfaceCount++;
            mainWindow->appendLog(QString("Found network interface: %1 (%2)").arg(name).arg(description), LogLevel::SUCCESS);
        }
    }
    
    // Enable/disable confirm button based on interface availability
    networkComps->confirmBtn->setEnabled(found);
    
    if (found) {
        networkComps->selector->setCurrentIndex(0);
        QString selectedInterface = networkComps->selector->currentData().toString();
        mainWindow->appendLog(QString("Network interface refresh completed, found %1 available interfaces").arg(interfaceCount), LogLevel::SUCCESS);
        mainWindow->appendLog(QString("Currently selected interface: %1").arg(selectedInterface), LogLevel::INFO);
    } else {
        mainWindow->appendLog("Warning: No available network interfaces found", LogLevel::WARNING);
        QMessageBox::warning(mainWindow, "Warning", "No available network interface found!");
    }
}

void NetworkManager::updateNetworkStatusIndicator(bool connected) {
    // Update network status indicator
    if (connected) {
        mainWindow->statusBar()->showMessage("Network connected", 2000);
        mainWindow->appendLog("Network connection status: Connected", LogLevel::SUCCESS);
    } else {
        mainWindow->statusBar()->showMessage("Network disconnected", 2000);
        mainWindow->appendLog("Network connection status: Disconnected", LogLevel::WARNING);
    }
}

void NetworkManager::onNetworkChanged(int index) {
    if (index >= 0) {
        QString selectedInterface = networkComps->selector->itemData(index).toString();
        mainWindow->appendLog(QString("Selected network interface: %1").arg(selectedInterface), LogLevel::INFO);
    }
}

void NetworkManager::onRefreshNetwork() {
    mainWindow->appendLog("User requested network interface refresh", LogLevel::INFO);
    refreshNetworkInterfaces();
}

void NetworkManager::onConfirmNetwork() {
    if (networkComps->selector->currentIndex() >= 0) {
        QString selectedInterface = networkComps->selector->currentData().toString();
        mainWindow->appendLog(QString("User confirmed network interface: %1").arg(selectedInterface), LogLevel::INFO);
        confirmNetworkInterface(selectedInterface);
    } else {
        mainWindow->appendLog("Error: Please select a network interface first", LogLevel::ERROR);
    }
}

void NetworkManager::confirmNetworkInterface(const QString& interfaceName) {
    mainWindow->appendLog(QString("Confirming network interface: %1").arg(interfaceName), LogLevel::INFO);
    
    // Disable network-related controls
    networkComps->selector->setEnabled(false);
    networkComps->refreshBtn->setEnabled(false);
    networkComps->confirmBtn->setEnabled(false);
    
    // Send signal to initialize EtherCAT thread
    sharedData.selectedInterface = interfaceName.toStdString();
    sharedData.interfaceConfirmed.store(true);
    
    mainWindow->appendLog(QString("Scanning slaves on network interface %1...").arg(interfaceName), LogLevel::INFO);
    
    // Update status bar
    mainWindow->statusBar()->showMessage(QString("Selected network interface: %1").arg(interfaceName), 2000);
    
    mainWindow->appendLog(QString("Network interface %1 confirmed successfully").arg(interfaceName), LogLevel::SUCCESS);
    
    // Can add more initialization code here
    // For example, start EtherCAT scanning thread, etc.
}