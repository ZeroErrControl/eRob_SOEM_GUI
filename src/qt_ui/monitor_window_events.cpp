#include "monitor_window_events.h"
#include <QMouseEvent>
#include <QThread>
#include <QTimer>
#include <QGuiApplication>
#include <QScreen>
#include "sdo_manager.h"
#include "ethercat.h"
#include "qcustomplot.h"

extern int dorun;
extern PDOManager::TxPDO txpdo;

MonitorWindowEvents::MonitorWindowEvents(QMainWindow* window,
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
                                       QDateTime startTime)
    : window(window), sharedData(sharedData), networkComps(networkComps),
      modeComps(modeComps), speedComps(speedComps), controlComps(controlComps),
      plotComps(plotComps), targetComps(targetComps), ppComps(ppComps),
      pvComps(pvComps), ptComps(ptComps), cstComps(cstComps),
      csvComps(csvComps), cspComps(cspComps),
      logDisplay(logDisplay), lockViewCheckBox(lockViewCheckBox),
      autoYRangeCheckBox(autoYRangeCheckBox), resetViewBtn(resetViewBtn),
      startTime(startTime) {
}

void MonitorWindowEvents::connectAllSignals() {
    // 图表显示控制
    connect(plotComps->velocityBtn, &QPushButton::toggled, this, &MonitorWindowEvents::onVelocityToggled);
    connect(plotComps->positionBtn, &QPushButton::toggled, this, &MonitorWindowEvents::onPositionToggled);
    connect(plotComps->torqueBtn, &QPushButton::toggled, this, &MonitorWindowEvents::onTorqueToggled);
    connect(plotComps->modeBtn, &QPushButton::toggled, this, &MonitorWindowEvents::onModeToggled);
    
    // 控制相关
    connect(speedComps->setBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onSpeedInputChanged);
    connect(controlComps->enableBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onEnableButtonClicked);
    connect(targetComps->setBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onTargetValueChanged);
    
    // 模式相关
    connect(modeComps->selector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MonitorWindowEvents::onModeChanged);
    connect(modeComps->confirmBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onModeConfirmClicked);
    connect(ppComps->confirmBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onPPParamsConfirmed);
    connect(pvComps->confirmBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onPVParamsConfirmed);
    connect(ptComps->confirmBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onPTParamsConfirmed);
    connect(cstComps->confirmBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onCSTParamsConfirmed);
    connect(cspComps->confirmBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onCSPParamsConfirmed);
    
    // 网络相关
    connect(networkComps->selector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MonitorWindowEvents::onNetworkChanged);
    connect(networkComps->refreshBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onRefreshNetwork);
    connect(networkComps->confirmBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onConfirmNetwork);
    
    // 视图控制
    connect(lockViewCheckBox, &QCheckBox::toggled, this, &MonitorWindowEvents::onLockViewToggled);
    connect(autoYRangeCheckBox, &QCheckBox::toggled, this, &MonitorWindowEvents::onAutoYRangeToggled);
    connect(resetViewBtn, &QPushButton::clicked, this, &MonitorWindowEvents::onResetViewClicked);
}

void MonitorWindowEvents::setupMouseEvents() {
    // 鼠标滚轮事件
    connect(plotComps->plot, &QCustomPlot::mouseWheel, this, &MonitorWindowEvents::onMouseWheel);
    
    // 鼠标移动事件
    connect(plotComps->plot, &QCustomPlot::mouseMove, this, &MonitorWindowEvents::onMouseMove);
    
    // 鼠标双击事件
    connect(plotComps->plot, &QCustomPlot::mouseDoubleClick, this, &MonitorWindowEvents::onMouseDoubleClick);
}

// Chart display control slot functions
void MonitorWindowEvents::onVelocityToggled(bool checked) {
    showVelocity = checked;
    plotComps->plot->graph(0)->setVisible(checked);
    updateYAxisRange();
    plotComps->plot->replot();
    
    if (checked) {
        appendLog("Velocity curve display enabled", LogLevel::INFO);
    } else {
        appendLog("Velocity curve display disabled", LogLevel::INFO);
    }
}

void MonitorWindowEvents::onPositionToggled(bool checked) {
    showPosition = checked;
    plotComps->plot->graph(1)->setVisible(checked);
    updateYAxisRange();
    plotComps->plot->replot();
    
    if (checked) {
        appendLog("Position curve display enabled", LogLevel::INFO);
    } else {
        appendLog("Position curve display disabled", LogLevel::INFO);
    }
}

void MonitorWindowEvents::onTorqueToggled(bool checked) {
    showTorque = checked;
    plotComps->plot->graph(2)->setVisible(checked);
    updateYAxisRange();
    plotComps->plot->replot();
    
    if (checked) {
        appendLog("Torque curve display enabled", LogLevel::INFO);
    } else {
        appendLog("Torque curve display disabled", LogLevel::INFO);
    }
}

void MonitorWindowEvents::onModeToggled(bool checked) {
    showMode = checked;
    plotComps->plot->graph(3)->setVisible(checked);
    updateYAxisRange();
    plotComps->plot->replot();
    
    if (checked) {
        appendLog("Mode curve display enabled", LogLevel::INFO);
    } else {
        appendLog("Mode curve display disabled", LogLevel::INFO);
    }
    
    // 更新状态栏
    if (checked) {
        QString modeText;
        switch (txpdo.mode_of_operation_display) {
            case 1: modeText = "PP"; break;
            case 3: modeText = "PV"; break;
            case 4: modeText = "PT"; break;
            case 8: modeText = "CSP"; break;
            case 9: modeText = "CSV"; break;
            case 10: modeText = "CST"; break;
            default: modeText = QString::number(txpdo.mode_of_operation_display);
        }
        updateStatusBar(QString("Current Operation Mode: %1").arg(modeText));
    }
}

// 控制相关槽函数
void MonitorWindowEvents::onSpeedInputChanged() {
    if (sharedData.motorEnabled.load()) {
        int newSpeed = speedComps->input->value();
        sharedData.targetVelocity.store(newSpeed);
        appendLog(QString("Setting new target velocity: %1").arg(newSpeed), LogLevel::SUCCESS);
        updateStatusBar(QString("Target velocity set to: %1").arg(newSpeed), 2000);
    } else {
        QMessageBox::warning(window, "Warning", "Please wait for motor enable to complete!");
        speedComps->input->setValue(0);
        appendLog("Warning: Motor not enabled, cannot set velocity", LogLevel::WARNING);
    }
}

void MonitorWindowEvents::onEnableButtonClicked() {
    if (!sharedData.modeConfirmed.load()) {
        QMessageBox::warning(window, "Warning", "Please confirm operation mode first!");
        controlComps->enableBtn->setChecked(false);
        appendLog("Warning: Operation mode not confirmed, cannot enable", LogLevel::WARNING);
        return;
    }
    
    if (controlComps->enableBtn->isChecked()) {
        sharedData.enableRequested.store(true);
        appendLog("Requesting motor enable", LogLevel::INFO);
    } else {
        sharedData.enableRequested.store(false);
        speedComps->input->setValue(0);
        sharedData.targetVelocity.store(0);
        appendLog("Requesting motor disable", LogLevel::INFO);
    }
}

void MonitorWindowEvents::onTargetValueChanged() {
    if (!sharedData.motorEnabled.load()) {
        QMessageBox::warning(window, "Warning", "Please enable motor first!");
        appendLog("Warning: Motor not enabled, cannot set target value", LogLevel::WARNING);
        return;
    }
    
    int currentMode = modeComps->selector->currentData().toInt();
    int value = targetComps->input->value();
    
    switch (currentMode) {
        case 1: // PP mode
        case 8: // CSP mode
            sharedData.targetPosition.store(value);
            appendLog(QString("Setting target position: %1").arg(value), LogLevel::SUCCESS);
            break;
            
        case 3: // PV mode
            if (!sharedData.pvParamsConfirmed.load()) {
                QMessageBox::warning(window, "Warning", "Please set PV mode parameters first!");
                appendLog("Warning: PV mode parameters not set", LogLevel::WARNING);
                return;
            }
            sharedData.targetVelocity.store(value);
            appendLog(QString("Setting PV mode target velocity: %1").arg(value), LogLevel::SUCCESS);
            break;
            
        case 9: // CSV mode
            sharedData.targetVelocity.store(value);
            appendLog(QString("Setting CSV mode target velocity: %1").arg(value), LogLevel::SUCCESS);
            break;
            
        case 4: // PT mode
        case 10: // CST mode
            sharedData.targetTorque.store(value);
            appendLog(QString("Setting target torque: %1").arg(value), LogLevel::SUCCESS);
            break;
    }
    
    updateStatusBar(QString("Target value set to: %1").arg(value), 2000);
}

// 模式相关槽函数
void MonitorWindowEvents::onModeChanged(int index) {
    int mode = modeComps->selector->currentData().toInt();
    
    // 电机使能时不允许切换模式
    if (sharedData.motorEnabled.load()) {
        QMessageBox::warning(window, "Warning", "Please disable motor first!");
        modeComps->selector->setCurrentIndex(modeComps->selector->findData(sharedData.operationMode.load()));
        return;
    }
    
    // 重置所有模式相关状态
    sharedData.modeConfirmed.store(false);
    sharedData.modeChangeRequested.store(false);
    sharedData.ppParamsConfirmed.store(false);
    sharedData.pvParamsConfirmed.store(false);
    sharedData.ptParamsConfirmed.store(false);
    sharedData.cstParamsConfirmed.store(false);
    
    // 重置目标输入状态
    if (targetComps && targetComps->input) {
        targetComps->input->setEnabled(false);
        targetComps->setBtn->setEnabled(false);
    }
    
    // 重置目标值
    targetComps->input->setValue(0);
    sharedData.targetPosition.store(0);
    sharedData.targetVelocity.store(0);
    sharedData.targetTorque.store(0);
    
    // 隐藏所有参数面板
    modeComps->paramsStack->setCurrentIndex(-1);
    
    // 根据模式显示相应的参数面板和设置
    updateModePanel(mode);
    
    updateStatusBar(QString("Selected %1").arg(modeComps->selector->currentText()), 2000);
}

void MonitorWindowEvents::onModeConfirmClicked() {
    if (!sharedData.motorEnabled.load()) {
        int mode = modeComps->selector->currentData().toInt();
        
        // 重置所有相关状态
        sharedData.modeConfirmed.store(false);
        sharedData.modeChangeRequested.store(false);
        sharedData.ppParamsConfirmed.store(false);
        sharedData.pvParamsConfirmed.store(false);
        sharedData.ptParamsConfirmed.store(false);
        sharedData.cstParamsConfirmed.store(false);
        
        // 重置目标值
        targetComps->input->setValue(0);
        sharedData.targetPosition.store(0);
        sharedData.targetVelocity.store(0);
        sharedData.targetTorque.store(0);
        
        // 通过PDO设置操作模式
        sharedData.operationMode.store(mode);
        sharedData.modeChangeRequested.store(true);
        
        appendLog(QString("Setting all slave operation mode: %1").arg(mode));
        
        // 等待所有从站切换模式
        int timeout = 50; // 5秒超时
        bool allModesChanged = false;
        
        while (timeout > 0 && !allModesChanged) {
            allModesChanged = true;
            // 检查所有从站的模式
            for (int slave = 1; slave <= ec_slavecount; slave++) {
                if (txpdo.mode_of_operation_display != mode) {
                    allModesChanged = false;
                    break;
                }
            }
            
            if (allModesChanged) {
                break;
            }
            QThread::msleep(100);
            timeout--;
        }
        
        if (allModesChanged) {
            sharedData.modeConfirmed.store(true);
            
            // 更新界面
            updateModePanel(mode);
            QString modeName = modeComps->selector->currentText();
            QString status = QString("All slaves switched to %1 (Mode: %2)").arg(modeName).arg(mode);
            updateStatusBar(status, 2000);
            appendLog(status);
            
            // 启用控制按钮
            controlComps->enableBtn->setEnabled(true);
            controlComps->enableBtn->setCheckable(true);
            modeComps->confirmBtn->setEnabled(false);
            modeComps->selector->setEnabled(false);
        } else {
            QString error = "Mode switching timeout for some or all slaves";
            appendLog("Error: " + error);
            QMessageBox::warning(window, "Error", error);
            
            // 重置状态
            sharedData.modeChangeRequested.store(false);
            sharedData.modeConfirmed.store(false);
            
            // 重新启用模式选择和确认按钮
            modeComps->selector->setEnabled(true);
            modeComps->confirmBtn->setEnabled(true);
        }
    } else {
        QMessageBox::warning(window, "Warning", "Please disable motor first!");
    }
}

// 参数确认槽函数
void MonitorWindowEvents::onPPParamsConfirmed() {
    int currentMode = modeComps->selector->currentData().toInt();
    if (currentMode != 1) {
        QMessageBox::warning(window, "Warning", "Can only set parameters in PP mode!");
        return;
    }

    if (!sharedData.motorEnabled.load()) {
        bool success = true;
        
        appendLog("Setting PP mode parameters...");
        
        // 创建参数结构
        SDOManager::PPParams params;
        params.velocity = ppComps->velocityInput->value();
        params.acceleration = ppComps->accelInput->value();
        params.deceleration = ppComps->decelInput->value();
        
        // 使用SDOManager配置参数
        auto& sdoManager = SDOManager::getInstance();
        
        // 为每个从站配置参数
        for (int slave = 1; slave <= ec_slavecount; slave++) {
            appendLog(QString("Setting parameters for slave %1...").arg(slave));
            
            if (!sdoManager.setPPModeParams(slave, params)) {
                success = false;
                appendLog(QString("Error: Failed to set PP mode parameters for slave %1: %2")
                    .arg(slave)
                    .arg(QString::fromStdString(sdoManager.getLastError())));
            }
        }
        
        if (success) {
            sharedData.ppParamsConfirmed.store(true);
            targetComps->input->setEnabled(true);
            targetComps->setBtn->setEnabled(true);
            
            appendLog("PP mode parameters set successfully");
            updateStatusBar("PP mode parameters set", 2000);
            
            // 保存参数到共享数据
            sharedData.ppParams.velocity = params.velocity;
            sharedData.ppParams.acceleration = params.acceleration;
            sharedData.ppParams.deceleration = params.deceleration;
        } else {
            QMessageBox::warning(window, "Warning", "Failed to set PP mode parameters, please check log");
        }
    } else {
        QMessageBox::warning(window, "Warning", "Please disable motor first!");
    }
}

void MonitorWindowEvents::onPVParamsConfirmed() {
    int currentMode = modeComps->selector->currentData().toInt();
    if (currentMode != 3) {
        QMessageBox::warning(window, "Warning", "Can only set parameters in PV mode!");
        return;
    }

    if (!sharedData.motorEnabled.load()) {
        bool success = true;
        
        appendLog("Setting PV mode parameters...");
        
        auto& sdoManager = SDOManager::getInstance();
        SDOManager::PVParams params;
        params.acceleration = pvComps->accelInput->value();
        params.deceleration = pvComps->decelInput->value();
        
        for (int slave = 1; slave <= ec_slavecount; slave++) {
            if (!sdoManager.setPVModeParams(slave, params)) {
                success = false;
                appendLog(QString("Error: Failed to set PV mode parameters for slave %1: %2")
                    .arg(slave)
                    .arg(QString::fromStdString(sdoManager.getLastError())));
            }
        }
        
        if (success) {
            sharedData.pvParamsConfirmed.store(true);
            targetComps->input->setEnabled(true);
            targetComps->setBtn->setEnabled(true);
            targetComps->label->setText("Target Velocity:");
            targetComps->input->setRange(-30000, 30000);
            targetComps->input->setSingleStep(100);
            
            appendLog("PV mode parameters set successfully");
            updateStatusBar("PV mode parameters set", 2000);
            
            sharedData.pvParams.acceleration = params.acceleration;
            sharedData.pvParams.deceleration = params.deceleration;
        } else {
            QMessageBox::warning(window, "Warning", "Failed to set PV mode parameters, please check log");
        }
    } else {
        QMessageBox::warning(window, "Warning", "Please disable motor first!");
    }
}

void MonitorWindowEvents::onPTParamsConfirmed() {
    int currentMode = modeComps->selector->currentData().toInt();
    if (currentMode != 4) {
        QMessageBox::warning(window, "Warning", "Can only set parameters in PT mode!");
        return;
    }

    if (!sharedData.motorEnabled.load()) {
        bool success = true;
        
        appendLog("Setting PT mode parameters...");
        
        auto& sdoManager = SDOManager::getInstance();
        SDOManager::PTParams params;
        params.max_torque = ptComps->maxTorqueInput->value();
        params.torque_slope = ptComps->torqueSlopeInput->value();
        
        for (int slave = 1; slave <= ec_slavecount; slave++) {
            if (!sdoManager.setPTModeParams(slave, params)) {
                success = false;
                appendLog(QString("Error: Failed to set PT mode parameters for slave %1: %2")
                    .arg(slave)
                    .arg(QString::fromStdString(sdoManager.getLastError())));
            }
        }
        
        if (success) {
            sharedData.ptParamsConfirmed.store(true);
            targetComps->input->setEnabled(true);
            targetComps->setBtn->setEnabled(true);
            targetComps->label->setText("Target Torque:");
            targetComps->input->setRange(-1000, 1000);
            targetComps->input->setSingleStep(10);
            
            appendLog("PT mode parameters set successfully");
            updateStatusBar("PT mode parameters set", 2000);
            
            sharedData.ptParams.max_torque = params.max_torque;
            sharedData.ptParams.torque_slope = params.torque_slope;
        } else {
            QMessageBox::warning(window, "Warning", "Failed to set PT mode parameters, please check log");
        }
    } else {
        QMessageBox::warning(window, "Warning", "Please disable motor first!");
    }
}

void MonitorWindowEvents::onCSTParamsConfirmed() {
    if (!sharedData.motorEnabled.load()) {
        SDOManager::CSTParams params;
        params.max_torque = cstComps->maxTorqueInput->value();
        params.torque_slope = cstComps->torqueSlopeInput->value();
        
        if (SDOManager::getInstance().setCSTParams(params)) {
            sharedData.cstParamsConfirmed.store(true);
            appendLog(QString("CST mode parameters set successfully - Max Torque: %1, Torque Slope: %2")
                .arg(params.max_torque)
                .arg(params.torque_slope));
            
            targetComps->input->setEnabled(true);
            targetComps->setBtn->setEnabled(true);
            
            cstComps->maxTorqueInput->setEnabled(false);
            cstComps->torqueSlopeInput->setEnabled(false);
            cstComps->confirmBtn->setEnabled(false);
            
            updateStatusBar("CST mode parameters set successfully", 2000);
        } else {
            QMessageBox::warning(window, "Warning", "Failed to set CST mode parameters!");
            appendLog("Warning: Failed to set CST mode parameters");
        }
    } else {
        QMessageBox::warning(window, "Warning", "Please disable motor first!");
    }
}

void MonitorWindowEvents::onCSPParamsConfirmed() {
    int currentMode = modeComps->selector->currentData().toInt();
    if (currentMode != 8) {
        QMessageBox::warning(window, "Warning", "Can only set parameters in CSP mode!");
        return;
    }
    if (!sharedData.motorEnabled.load()) {
        int maxVel = cspComps->velocityInput->value();
        sharedData.cspMaxVelocity = maxVel;
        targetComps->input->setEnabled(true);
        targetComps->setBtn->setEnabled(true);
        appendLog(QString("CSP mode max velocity set: %1").arg(maxVel), LogLevel::SUCCESS);
        updateStatusBar("CSP mode parameters set", 2000);
    } else {
        QMessageBox::warning(window, "Warning", "Please disable motor first!");
    }
}

// 网络相关槽函数
void MonitorWindowEvents::onNetworkChanged(int index) {
    if (index >= 0) {
        QString selectedInterface = networkComps->selector->itemData(index).toString();
        appendLog(QString("Switched to network interface: %1").arg(selectedInterface), LogLevel::INFO);
        
        if (sharedData.motorEnabled.load()) {
            QMessageBox::warning(window, "Warning", 
                "Changing network interface requires restarting the program.\nPlease disable motor and restart the program first.");
            appendLog("Warning: Motor enabled, cannot change network interface", LogLevel::WARNING);
            return;
        }
        
        updateStatusBar(QString("Selected network interface: %1").arg(selectedInterface), 2000);
    }
}

void MonitorWindowEvents::onRefreshNetwork() {
    appendLog("User requested network interface refresh", LogLevel::INFO);
    // 这里应该调用NetworkManager的刷新方法
    updateStatusBar("Refreshing network interfaces...", 2000);
}

void MonitorWindowEvents::onConfirmNetwork() {
    QString selectedInterface = networkComps->selector->currentData().toString();
    appendLog(QString("User confirmed network interface: %1").arg(selectedInterface), LogLevel::INFO);
    
    // 禁用网络相关控件
    networkComps->selector->setEnabled(false);
    networkComps->refreshBtn->setEnabled(false);
    networkComps->confirmBtn->setEnabled(false);
    
    // 发送信号初始化EtherCAT线程
    sharedData.selectedInterface = selectedInterface.toStdString();
    sharedData.interfaceConfirmed.store(true);
    
    appendLog(QString("Scanning slaves on network interface %1...").arg(selectedInterface), LogLevel::INFO);
    
    // 这里应该调用NetworkManager的确认方法
    appendLog(QString("Network interface %1 confirmed successfully").arg(selectedInterface), LogLevel::SUCCESS);
    updateStatusBar("Network interface confirmed", 2000);
}

// 视图控制槽函数
void MonitorWindowEvents::onLockViewToggled(bool checked) {
    lockView = checked;
    
    if (checked) {
        appendLog("Chart view locked", LogLevel::INFO);
    } else {
        appendLog("Chart view unlocked", LogLevel::INFO);
        double key = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0 - startTime.toMSecsSinceEpoch() / 1000.0;
        double rangeSize = plotComps->plot->xAxis->range().size();
        plotComps->plot->xAxis->setRange(key, rangeSize, Qt::AlignRight);
        plotComps->plot->replot();
    }
}

void MonitorWindowEvents::onAutoYRangeToggled(bool checked) {
    autoYRange = checked;
    if (checked) {
        appendLog("Y-axis auto-range enabled", LogLevel::INFO);
        updateYAxisRange(true);
        plotComps->plot->replot();
    } else {
        appendLog("Y-axis auto-range disabled", LogLevel::INFO);
    }
}

void MonitorWindowEvents::onResetViewClicked() {
    appendLog("User clicked reset view button", LogLevel::INFO);
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0 - startTime.toMSecsSinceEpoch() / 1000.0;
    plotComps->plot->xAxis->setRange(key, 8, Qt::AlignRight);
    updateYAxisRange(true);
    plotComps->plot->replot();
    
    lockViewCheckBox->setChecked(false);
    appendLog("Chart view reset", LogLevel::SUCCESS);
    updateStatusBar("Chart view reset", 2000);
}

void MonitorWindowEvents::onMouseWheel() {
    lockViewCheckBox->setChecked(true);
    autoYRangeCheckBox->setChecked(false);
    appendLog("鼠标滚轮操作已锁定视图", LogLevel::INFO);
}

void MonitorWindowEvents::onMouseMove(QMouseEvent* event) {
    if (event->buttons() != Qt::NoButton) {
        lockViewCheckBox->setChecked(true);
        appendLog("鼠标拖拽操作已锁定视图", LogLevel::INFO);
    }
}

void MonitorWindowEvents::onMouseDoubleClick() {
    appendLog("用户双击图表，重置视图", LogLevel::INFO);
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0 - startTime.toMSecsSinceEpoch() / 1000.0;
    plotComps->plot->xAxis->setRange(key, 8, Qt::AlignRight);
    updateYAxisRange(true);
    plotComps->plot->replot();
    lockViewCheckBox->setChecked(false);
    
    appendLog("图表视图已通过双击重置", LogLevel::SUCCESS);
    updateStatusBar("Chart view reset", 2000);
}

// 辅助方法
void MonitorWindowEvents::updateYAxisRange(bool forceUpdate) {
    if (autoYRange || forceUpdate) {
        plotComps->plot->rescaleAxes();
    }
}

void MonitorWindowEvents::appendLog(const QString& message, LogLevel level) {
    if (logDisplay) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        QString levelStr;
        switch (level) {
            case LogLevel::INFO: levelStr = "INFO"; break;
            case LogLevel::WARNING: levelStr = "WARN"; break;
            case LogLevel::ERROR: levelStr = "ERROR"; break;
            case LogLevel::SUCCESS: levelStr = "SUCCESS"; break;
        }
        
        QString logMessage = QString("[%1] [%2] %3").arg(timestamp, levelStr, message);
        logDisplay->append(logMessage);
    }
}

void MonitorWindowEvents::updateModePanel(int mode) {
    switch (mode) {
        case 1:  // PP mode
            modeComps->paramsStack->setCurrentWidget(ppComps->panel);
            targetComps->container->show();
            targetComps->label->setText("目标位置:");
            targetComps->input->setRange(-1000000, 1000000);
            targetComps->input->setSingleStep(1000);
            targetComps->input->setEnabled(sharedData.ppParamsConfirmed.load());
            targetComps->setBtn->setEnabled(sharedData.ppParamsConfirmed.load());
            break;
            
        case 3:  // PV mode
            modeComps->paramsStack->setCurrentWidget(pvComps->panel);
            targetComps->container->show();
            targetComps->label->setText("Target Velocity:");
            targetComps->input->setRange(-50000, 50000);
            targetComps->input->setSingleStep(100);
            targetComps->input->setEnabled(sharedData.pvParamsConfirmed.load());
            targetComps->setBtn->setEnabled(sharedData.pvParamsConfirmed.load());
            break;
            
        case 4:  // PT mode
            modeComps->paramsStack->setCurrentWidget(ptComps->panel);
            targetComps->container->show();
            targetComps->label->setText("Target Torque:");
            targetComps->input->setRange(-100, 100);
            targetComps->input->setSingleStep(10);
            targetComps->input->setEnabled(sharedData.ptParamsConfirmed.load());
            targetComps->setBtn->setEnabled(sharedData.ptParamsConfirmed.load());
            break;
            
        case 8:  // CSP mode
            modeComps->paramsStack->setCurrentWidget(cspComps->panel);
            targetComps->container->show();
            targetComps->label->setText("Target Position:");
            targetComps->input->setRange(-1000000, 1000000);
            targetComps->input->setSingleStep(1000);
            targetComps->input->setEnabled(true);
            targetComps->setBtn->setEnabled(true);
            break;
            
        case 9:  // CSV mode
            modeComps->paramsStack->setCurrentWidget(csvComps->panel);
            targetComps->container->show();
            targetComps->label->setText("Target Velocity:");
            targetComps->input->setRange(-30000, 30000);
            targetComps->input->setSingleStep(100);
            targetComps->input->setEnabled(true);
            targetComps->setBtn->setEnabled(true);
            break;
            
        case 10: // CST mode
            modeComps->paramsStack->setCurrentWidget(cstComps->panel);
            targetComps->container->show();
            targetComps->label->setText("Target Torque:");
            targetComps->input->setRange(-100, 100);
            targetComps->input->setSingleStep(10);
            targetComps->input->setEnabled(sharedData.cstParamsConfirmed.load());
            targetComps->setBtn->setEnabled(sharedData.cstParamsConfirmed.load());
            
            cstComps->maxTorqueInput->setEnabled(!sharedData.cstParamsConfirmed.load());
            cstComps->torqueSlopeInput->setEnabled(!sharedData.cstParamsConfirmed.load());
            cstComps->confirmBtn->setEnabled(!sharedData.cstParamsConfirmed.load());
            break;
    }
}

void MonitorWindowEvents::updateStatusBar(const QString& message, int timeout) {
    if (window && window->statusBar()) {
        window->statusBar()->showMessage(message, timeout);
    }
} 