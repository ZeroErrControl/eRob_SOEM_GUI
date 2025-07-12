#include "qt_ui/control/motor_controller.h"
#include "monitor_window.h"

MotorController::MotorController(MonitorWindow* parent, monitor::SharedData& sharedData,
                               ComponentManager::ControlComponents* controlComps,
                               ComponentManager::TargetComponents* targetComps)
    : QObject(parent), mainWindow(parent), sharedData(sharedData),
      controlComps(controlComps), targetComps(targetComps) {
    
    // No need to get components again since they are passed as parameters
}

void MotorController::enableMotor() {
    // Motor enable logic
    mainWindow->appendLog("Requesting motor enable...", LogLevel::INFO);
    sharedData.enableRequested.store(true);
    controlComps->enableBtn->setText("Enabling...");
    controlComps->enableBtn->setEnabled(false);
    mainWindow->appendLog("Motor enable request sent", LogLevel::SUCCESS);
}

void MotorController::disableMotor() {
    // Motor disable logic
    mainWindow->appendLog("Requesting motor disable...", LogLevel::INFO);
    sharedData.enableRequested.store(false);
    controlComps->enableBtn->setText("Disabling...");
    controlComps->enableBtn->setEnabled(false);
    sharedData.targetVelocity.store(0);
    mainWindow->appendLog("Motor disable request sent", LogLevel::SUCCESS);
}

void MotorController::onEnableButtonClicked() {
    // Enable button handling
    if (!sharedData.modeConfirmed.load()) {
        QMessageBox::warning(mainWindow, "Warning", "Please confirm operation mode first!");
        controlComps->enableBtn->setChecked(false);
        mainWindow->appendLog("Warning: Operation mode not confirmed, cannot enable motor", LogLevel::WARNING);
        return;
    }
    
    if (controlComps->enableBtn->isChecked()) {
        enableMotor();
    } else {
        disableMotor();
    }
}

void MotorController::updateTargetValue(int value) {
    // Update target value
    QString modeName;
    switch (sharedData.operationMode.load()) {
        case 1: // PP
            sharedData.targetPosition.store(value);
            modeName = "PP Mode";
            break;
        case 3: // PV
            sharedData.targetVelocity.store(value);
            sharedData.pvNewTarget.store(true);
            modeName = "PV Mode";
            break;
        case 4: // PT
            sharedData.targetTorque.store(value);
            modeName = "PT Mode";
            break;
        case 8: // CSP
            sharedData.targetPosition.store(value);
            modeName = "CSP Mode";
            break;
        case 9: // CSV
            sharedData.targetVelocity.store(value);
            modeName = "CSV Mode";
            break;
        case 10: // CST
            sharedData.targetTorque.store(value);
            modeName = "CST Mode";
            break;
        default:
            modeName = "Unknown Mode";
            break;
    }
    
    mainWindow->appendLog(QString("%1 target value updated: %2").arg(modeName).arg(value), LogLevel::SUCCESS);
}

void MotorController::updateEnableButtonState() {
    // Update enable button state
    bool wasEnabled = controlComps->enableBtn->isChecked();
    bool isCurrentlyEnabled = sharedData.motorEnabled.load();
    
    if (isCurrentlyEnabled) {
        controlComps->enableBtn->setText("Disable Motor");
        controlComps->enableBtn->setChecked(true);
        controlComps->enableBtn->setEnabled(true);
    } else {
        controlComps->enableBtn->setText("Enable Motor");
        controlComps->enableBtn->setChecked(false);
        controlComps->enableBtn->setEnabled(true);
    }
    
    // Add debug information (optional, for development debugging)
    // mainWindow->appendLog(QString("Motor state update: Currently enabled=%1, Previous button state=%2")
    //                      .arg(isCurrentlyEnabled ? "Yes" : "No")
    //                      .arg(wasEnabled ? "Yes" : "No"), LogLevel::INFO);
}

void MotorController::updatePositionStatus(int32_t currentPosition, int32_t targetPosition) {
    static const int POSITION_TOLERANCE = 20;  // Position tolerance
    static bool reachedTarget = false;  // Moved to function beginning
    
    QString status = QString("Target Position: %1, Current Position: %2, Error: %3")
                     .arg(targetPosition)
                     .arg(currentPosition)
                     .arg(abs(currentPosition - targetPosition));
                     
    mainWindow->statusBar()->showMessage(status);
    
    // Update target value input box visual feedback in position mode
    int currentMode = sharedData.operationMode.load();
    if (currentMode == 1 || currentMode == 8) {  // PP or CSP mode
        if (abs(currentPosition - targetPosition) < POSITION_TOLERANCE) {
            targetComps->input->setStyleSheet("QSpinBox { background-color: lightgreen; }");
            // Only record when target is reached for the first time
            if (!reachedTarget) {
                mainWindow->appendLog("Position target achieved", LogLevel::SUCCESS);
                reachedTarget = true;
            }
        } else {
            targetComps->input->setStyleSheet("");
            reachedTarget = false;
        }
    }
}

void MotorController::onTargetValueChanged() {
    // Handle target value change
    if (!sharedData.motorEnabled.load()) {
        QMessageBox::warning(mainWindow, "Warning", "Please enable motor first!");
        mainWindow->appendLog("Warning: Motor not enabled, cannot set target value", LogLevel::WARNING);
        return;
    }
    
    int value = targetComps->input->value();
    updateTargetValue(value);
    
    mainWindow->appendLog(QString("Target value set: %1").arg(value), LogLevel::SUCCESS);
}