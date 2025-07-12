#include "qt_ui/control/mode_manager.h"
#include "monitor_window.h"

ModeManager::ModeManager(MonitorWindow* parent, monitor::SharedData& sharedData, 
                       ComponentManager::ModeComponents* modeComps,
                       ComponentManager::PPComponents* ppComps,
                       ComponentManager::PVComponents* pvComps,
                       ComponentManager::PTComponents* ptComps,
                       ComponentManager::CSTComponents* cstComps,
                       ComponentManager::CSVComponents* csvComps,
                       ComponentManager::CSPComponents* cspComps,
                       ComponentManager::TargetComponents* targetComps)
    : QObject(parent), mainWindow(parent), sharedData(sharedData),
      modeComps(modeComps), ppComps(ppComps), pvComps(pvComps),
      ptComps(ptComps), cstComps(cstComps), csvComps(csvComps),
      cspComps(cspComps), targetComps(targetComps) {
    
    // Constructor body, can be empty or add initialization code
}

void ModeManager::updateModePanel(int mode) {
    // Update mode panel
    QString modeName;
    switch (mode) {
        case 1: // PP Mode
            modeComps->paramsStack->setCurrentWidget(ppComps->panel);
            modeName = "PP Mode";
            break;
        case 3: // PV Mode
            modeComps->paramsStack->setCurrentWidget(pvComps->panel);
            modeName = "PV Mode";
            break;
        case 4: // PT Mode
            modeComps->paramsStack->setCurrentWidget(ptComps->panel);
            modeName = "PT Mode";
            break;
        case 8: // CSP Mode
            modeComps->paramsStack->setCurrentWidget(cspComps->panel);
            modeName = "CSP Mode";
            break;
        case 9: // CSV Mode
            modeComps->paramsStack->setCurrentWidget(csvComps->panel);
            modeName = "CSV Mode";
            break;
        case 10: // CST Mode
            modeComps->paramsStack->setCurrentWidget(cstComps->panel);
            modeName = "CST Mode";
            break;
        default:
            // Default CSV Mode
            modeComps->paramsStack->setCurrentWidget(csvComps->panel);
            modeName = "CSV Mode";
            break;
    }
    mainWindow->appendLog(QString("Switched to %1 parameter panel").arg(modeName), LogLevel::INFO);
}

void ModeManager::updateModeStatusIndicator() {
    // Update status indicator based on current mode
    int mode = sharedData.operationMode.load();
    QString modeText;
    
    switch (mode) {
        case 1: modeText = "PP"; break;
        case 3: modeText = "PV"; break;
        case 4: modeText = "PT"; break;
        case 8: modeText = "CSP"; break;
        case 9: modeText = "CSV"; break;
        case 10: modeText = "CST"; break;
        default: modeText = QString::number(mode);
    }
    
    mainWindow->statusBar()->showMessage(QString("Current operation mode: %1").arg(modeText), 2000);
    mainWindow->appendLog(QString("Current operation mode: %1").arg(modeText), LogLevel::INFO);
}

void ModeManager::switchToMode(int mode) {
    // Implementation of switching to specified mode
    mainWindow->appendLog(QString("Switching to mode: %1").arg(mode), LogLevel::INFO);
    sharedData.operationMode.store(mode);
    sharedData.modeChangeRequested.store(true);
    mainWindow->appendLog(QString("Mode switch request sent: %1").arg(mode), LogLevel::SUCCESS);
}

void ModeManager::confirmMode() {
    // Implementation of confirming current mode
    int mode = modeComps->selector->currentData().toInt();
    mainWindow->appendLog(QString("Confirming mode: %1").arg(mode), LogLevel::INFO);
    switchToMode(mode);
}

void ModeManager::confirmPPParams() {
    // Implementation of confirming PP mode parameters
    // ...implementation code...
}

// Slot function implementations
void ModeManager::onModeChanged(int index) {
    // Implementation of handling mode change
    int mode = modeComps->selector->currentData().toInt();
    mainWindow->appendLog(QString("User selected mode: %1").arg(mode), LogLevel::INFO);
    updateModePanel(mode);
}

void ModeManager::onModeConfirmClicked() {
    // Implementation of handling mode confirm button click
    mainWindow->appendLog("User clicked mode confirm button", LogLevel::INFO);
    confirmMode();
}

void ModeManager::onPPParamsConfirmed() {
    // PP mode parameter confirmation
    mainWindow->appendLog("Confirming PP mode parameters...", LogLevel::INFO);
    
    if (!sharedData.motorEnabled.load()) {
        // Get parameter values
        int velocity = ppComps->velocityInput->value();
        int accel = ppComps->accelInput->value();
        int decel = ppComps->decelInput->value();
        
        // Store parameters
        sharedData.ppParams.velocity = velocity;
        sharedData.ppParams.acceleration = accel;
        sharedData.ppParams.deceleration = decel;
        sharedData.ppParamsConfirmed.store(true);
        
        // Update UI
        mainWindow->appendLog(QString("PP mode parameters confirmed: Velocity=%1, Acceleration=%2, Deceleration=%3")
                            .arg(velocity).arg(accel).arg(decel), LogLevel::SUCCESS);
        
        // Enable target value input
        targetComps->input->setEnabled(true);
        targetComps->setBtn->setEnabled(true);
        mainWindow->appendLog("PP mode target value input enabled", LogLevel::SUCCESS);
    } else {
        mainWindow->appendLog("Cannot change parameters: Motor is enabled", LogLevel::WARNING);
    }
}

void ModeManager::onPVParamsConfirmed() {
    // PV mode parameter confirmation
    mainWindow->appendLog("Confirming PV mode parameters...", LogLevel::INFO);
    
    if (!sharedData.motorEnabled.load()) {
        // Get parameter values
        int accel = pvComps->accelInput->value();
        int decel = pvComps->decelInput->value();
        
        // Store parameters
        sharedData.pvParams.acceleration = accel;
        sharedData.pvParams.deceleration = decel;
        sharedData.pvParamsConfirmed.store(true);
        
        // Update UI
        mainWindow->appendLog(QString("PV mode parameters confirmed: Acceleration=%1, Deceleration=%2")
                            .arg(accel).arg(decel), LogLevel::SUCCESS);
        
        // Enable target value input
        targetComps->input->setEnabled(true);
        targetComps->setBtn->setEnabled(true);
        mainWindow->appendLog("PV mode target value input enabled", LogLevel::SUCCESS);
    } else {
        mainWindow->appendLog("Cannot change parameters: Motor is enabled", LogLevel::WARNING);
    }
}

void ModeManager::onPTParamsConfirmed() {
    // PT mode parameter confirmation
    mainWindow->appendLog("Confirming PT mode parameters...", LogLevel::INFO);
    
    if (!sharedData.motorEnabled.load()) {
        // Get parameter values
        int maxTorque = ptComps->maxTorqueInput->value();
        int torqueSlope = ptComps->torqueSlopeInput->value();
        
        // Store parameters
        sharedData.ptParams.max_torque = maxTorque;
        sharedData.ptParams.torque_slope = torqueSlope;
        sharedData.ptParamsConfirmed.store(true);
        
        // Update UI
        mainWindow->appendLog(QString("PT mode parameters confirmed: Max Torque=%1, Torque Slope=%2")
                            .arg(maxTorque).arg(torqueSlope), LogLevel::SUCCESS);
        
        // Enable target value input
        targetComps->input->setEnabled(true);
        targetComps->setBtn->setEnabled(true);
        mainWindow->appendLog("PT mode target value input enabled", LogLevel::SUCCESS);
    } else {
        mainWindow->appendLog("Cannot change parameters: Motor is enabled", LogLevel::WARNING);
    }
}

void ModeManager::onCSTParamsConfirmed() {
    // CST mode parameter confirmation
    mainWindow->appendLog("Confirming CST mode parameters...", LogLevel::INFO);
    
    if (!sharedData.motorEnabled.load()) {
        // Get parameter values
        int maxTorque = cstComps->maxTorqueInput->value();
        int torqueSlope = cstComps->torqueSlopeInput->value();
        
        // Store parameters
        sharedData.cstParams.max_torque = maxTorque;
        sharedData.cstParams.torque_slope = torqueSlope;
        sharedData.cstParamsConfirmed.store(true);
        
        // Update UI
        mainWindow->appendLog(QString("CST mode parameters confirmed: Max Torque=%1, Torque Slope=%2")
                            .arg(maxTorque).arg(torqueSlope), LogLevel::SUCCESS);
        
        // Enable target value input
        targetComps->input->setEnabled(true);
        targetComps->setBtn->setEnabled(true);
        mainWindow->appendLog("CST mode target value input enabled", LogLevel::SUCCESS);
    } else {
        mainWindow->appendLog("Cannot change parameters: Motor is enabled", LogLevel::WARNING);
    }
}

// Other slot function implementations...