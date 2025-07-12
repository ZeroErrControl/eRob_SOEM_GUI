#include "monitor_window.h"
#include "monitor_window_ui.h"
#include "monitor_window_events.h"
#include "monitor_window_data.h"
#include <QDateTime>
#include "ethercat.h"
#include <QSplitter>
#include <QtNetwork/QNetworkInterface>
#include <QStatusBar>
#include <QMessageBox>
#include <QToolBar>
#include <QStyle>
#include <QDesktopWidget>
#include <QTabWidget>
#include "sdo_manager.h"
#include "component_manager.h"
#include <QGuiApplication>
#include <QScreen>
#include "plot/plot_manager.h"
#include "control/motor_controller.h"
#include "control/mode_manager.h"
#include "network/network_manager.h"
#include "logging/log_manager.h"
#include "ethercat_thread.h"
#include <QCoreApplication>
#include <QTimer>

// External variables declaration
extern int dorun;
extern PDOManager::TxPDO txpdo;

MonitorWindow::MonitorWindow(monitor::SharedData& data, QWidget *parent) : QMainWindow(parent), sharedData(data), running(true), 
      showVelocity(true), showPosition(false), showTorque(false),
      showMode(true) {
    QStringList earlyLogs;
    earlyLogs << "Initializing monitor window...";
    
    // Get component manager instance and initialize all component pointers
    auto& compManager = ComponentManager::getInstance();
    earlyLogs << "Component manager acquired";
    
    // Initialize all component pointers
    networkComps = compManager.createNetworkComponents(this);
    modeComps = compManager.createModeComponents(this);
    ppComps = compManager.createPPComponents(this);
    speedComps = compManager.createSpeedComponents(this);
    controlComps = compManager.createControlComponents(this);
    plotComps = compManager.createPlotComponents(this);
    targetComps = compManager.createTargetComponents(this);
    
    // initialize other mode components
    csvComps = compManager.createCSVComponents(this);
    cspComps = compManager.createCSPComponents(this);
    cstComps = compManager.createCSTComponents(this);
    pvComps = compManager.createPVComponents(this);
    ptComps = compManager.createPTComponents(this);
    
    earlyLogs << "All UI components created";
    
    // Use UI class to create layout
    startTime = QDateTime::currentDateTime();
    auto uiElements = MonitorWindowUI::createMainLayout(this, networkComps, modeComps, speedComps, 
                                                       controlComps, plotComps, targetComps, ppComps, 
                                                       pvComps, ptComps, cstComps, csvComps, cspComps);
    
    earlyLogs << "Main layout created";
    
    // Save UI element references
    logDisplay = uiElements.logDisplay;
    lockViewCheckBox = uiElements.lockViewCheckBox;
    autoYRangeCheckBox = uiElements.autoYRangeCheckBox;
    resetViewBtn = uiElements.resetViewBtn;
    
    // Set chart controls
    MonitorWindowUI::setupChartControls(plotComps, uiElements.chartControlBox, 
                                       lockViewCheckBox, autoYRangeCheckBox, resetViewBtn, this);
    
    earlyLogs << "Chart control set up";
    
    // Create logManager (early)
    logManager = new LogManager(this, logDisplay);
    
    // Output early logs
    for (const auto& msg : earlyLogs) {
        appendLog(msg, LogLevel::INFO);
    }
    earlyLogs.clear();
    
    // Create event handler
    eventsHandler = new MonitorWindowEvents(this, sharedData, networkComps, modeComps, speedComps,
                                          controlComps, plotComps, targetComps, ppComps, pvComps,
                                          ptComps, cstComps, csvComps, cspComps, logDisplay, lockViewCheckBox,
                                          autoYRangeCheckBox, resetViewBtn, startTime);
    appendLog("Event handler created", LogLevel::SUCCESS);
    
    // Connect all signals
    eventsHandler->connectAllSignals();
    eventsHandler->setupMouseEvents();
    appendLog("All signals connected", LogLevel::SUCCESS);
    
    // Create data manager
    dataManager = new MonitorWindowData(this, sharedData, plotComps, startTime);
    
    // Create function managers
    plotManager = new PlotManager(this, plotComps);
    motorController = new MotorController(this, sharedData, controlComps, targetComps);
    modeManager = new ModeManager(this, sharedData, modeComps, ppComps, pvComps, ptComps, cstComps, csvComps, cspComps, targetComps);
    networkManager = new NetworkManager(this, networkComps);
    appendLog("All managers created", LogLevel::SUCCESS);
    
    // Create EtherCAT thread, but do not start
    ethercat_thread = new EtherCATThread(this);
    appendLog("EtherCAT thread created", LogLevel::SUCCESS);

    // Start EtherCAT thread
    ethercat_thread->start();
    appendLog("EtherCAT thread started", LogLevel::SUCCESS);

    // Connect signals and slots
    connectSignals();
    
    // Start data timer
    startDataTimer();

    // Start state check timer
    startStateCheckTimer();

    // Initialize network interface list
    networkManager->refreshNetworkInterfaces();
    
    // Set window properties
    setWindowTitle("EtherCAT Servo Motor Control and Monitoring System");
    resize(1200, 800);
    
    // Center window
    QRect screenGeometry;
    if (QGuiApplication::screens().size() > 0) {
        screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    } else {
        screenGeometry = QRect(0, 0, 1200, 800);
    }
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // Apply global style
    ComponentManager::getInstance().applyGlobalStyle();
    
    // Create status bar
    statusBar()->setStyleSheet("QStatusBar::item { border: none; }");
    statusBar()->showMessage("System ready");
    
    // Initialize member variables
    this->lockView = false;
    this->autoYRange = true;
    
    appendLog("Monitor window initialized", LogLevel::SUCCESS);
}

void MonitorWindow::connectSignals() {
    // Connect EtherCAT thread signals
    connect(ethercat_thread, &EtherCATThread::motorStateChanged, this, &MonitorWindow::updateEnableButtonState);
    appendLog("EtherCAT thread signals connected", LogLevel::SUCCESS);
}

void MonitorWindow::startDataTimer() {
    if (dataManager) {
        dataManager->startDataTimer();
    }
}

void MonitorWindow::startStateCheckTimer() {
    // Create state check timer
    QTimer* stateCheckTimer = new QTimer(this);
    connect(stateCheckTimer, &QTimer::timeout, this, &MonitorWindow::checkMotorStateChange);
    stateCheckTimer->start(100); // Check state every 100ms
}

void MonitorWindow::checkMotorStateChange() {
    // Check if motor state has changed
    extern volatile bool motorStateChanged;
    extern volatile bool lastMotorEnabled;
    
    if (motorStateChanged) {
        motorStateChanged = false; // Reset flag
        
        // Update UI state
        updateEnableButtonState();
        
        // Log based on current state
        if (lastMotorEnabled) {
            appendLog("Motor enabled successfully!", LogLevel::SUCCESS);
            statusBar()->showMessage("Motor enabled", 3000);
        } else {
            appendLog("Motor disabled successfully!", LogLevel::SUCCESS);
            statusBar()->showMessage("Motor disabled", 3000);
        }
    }
}

// Public interface methods
void MonitorWindow::appendLog(const QString& message, LogLevel level) {
    if (logManager) {
        logManager->appendLog(message, level);
    }
}

// Proxy method - forward to appropriate manager
void MonitorWindow::updateYAxisRange(bool forceUpdate) {
    if (plotManager) {
        plotManager->updateYAxisRange(forceUpdate);
    }
}

void MonitorWindow::refreshNetworkInterfaces() {
    if (networkManager) {
        networkManager->refreshNetworkInterfaces();
    }
}

void MonitorWindow::updateModePanel(int mode) {
    if (modeManager) {
        modeManager->updateModePanel(mode);
    }
}

void MonitorWindow::updatePositionStatus(int32_t currentPosition, int32_t targetPosition) {
    if (motorController) {
        motorController->updatePositionStatus(currentPosition, targetPosition);
    }
}

void MonitorWindow::updateEnableButtonState() {
    if (motorController) {
        motorController->updateEnableButtonState();
    }
}

void MonitorWindow::updateNetworkStatusIndicator(bool connected) {
    if (networkManager) {
        networkManager->updateNetworkStatusIndicator(connected);
    }
}

void MonitorWindow::updateModeStatusIndicator() {
    if (modeManager) {
        modeManager->updateModeStatusIndicator();
    }
}

// Implement close event handler
void MonitorWindow::closeEvent(QCloseEvent *event) {
    appendLog("User requested application closure", LogLevel::INFO);
    appendLog("Stopping all threads and timers...", LogLevel::INFO);
    
    // Immediately hide window
    this->hide();
    event->accept();
    
    // Asynchronous cleanup and exit
    QTimer::singleShot(0, [this]() {
        // First stop data timer
        if (dataManager) {
            dataManager->stopDataTimer();
            appendLog("Data timer stopped", LogLevel::SUCCESS);
        }
        
        // Set stop flag to notify EtherCAT thread to stop
        sharedData.isRunning.store(false);
        dorun = 0;  // Set global stop flag
        
        appendLog("Stop flag set, waiting for EtherCAT thread to stop...", LogLevel::INFO);
        
        // Stop EtherCAT thread and wait
        if (ethercat_thread && ethercat_thread->isRunning()) {
            // Only set stop flag, let thread exit naturally
            sharedData.isRunning.store(false);
            dorun = 0;
            
            // Wait for EtherCAT thread to stop, with timeout
            if (!ethercat_thread->wait(3000)) {  // 3 second timeout
                appendLog("Warning: EtherCAT thread did not stop within 3 seconds, forcing termination", LogLevel::WARNING);
                ethercat_thread->terminate();
                ethercat_thread->wait(1000);  // Wait another second
            } else {
                appendLog("EtherCAT thread stopped normally", LogLevel::SUCCESS);
            }
        }
        
        appendLog("All threads and timers stopped", LogLevel::SUCCESS);
        appendLog("Application will close", LogLevel::INFO);
        
        // Emit window closed signal
        emit windowClosed();
        
        // Exit application
        QCoreApplication::quit();
    });
}

// Slot functions - now just simple proxies
void MonitorWindow::updatePlot() {
    // Data updates are now handled by MonitorWindowData
}

void MonitorWindow::onVelocityToggled(bool checked) {
    showVelocity = checked;
    if (plotManager) {
        // Directly call chart methods
    plotComps->plot->graph(0)->setVisible(checked);
    updateYAxisRange();
    plotComps->plot->replot();
    }
}

void MonitorWindow::onPositionToggled(bool checked) {
    showPosition = checked;
    if (plotManager) {
    plotComps->plot->graph(1)->setVisible(checked);
    updateYAxisRange();
    plotComps->plot->replot();
    }
}

void MonitorWindow::onTorqueToggled(bool checked) {
    showTorque = checked;
    if (plotManager) {
    plotComps->plot->graph(2)->setVisible(checked);
    updateYAxisRange();
    plotComps->plot->replot();
    }
}

void MonitorWindow::onSpeedInputChanged() {
    if (motorController) {
        // 直接处理速度输入变化
    if (sharedData.motorEnabled.load()) {
        int newSpeed = speedComps->input->value();
        sharedData.targetVelocity.store(newSpeed);
        statusBar()->showMessage(QString("Target velocity set to: %1").arg(newSpeed), 2000);
    } else {
        QMessageBox::warning(this, "Warning", "Please wait for motor enable to complete!");
        speedComps->input->setValue(0);
        }
    }
}

void MonitorWindow::onEnableButtonClicked() {
    if (motorController) {
        // 直接处理使能按钮点击
    if (!sharedData.modeConfirmed.load()) {
        QMessageBox::warning(this, "Warning", "Please confirm operation mode first!");
        controlComps->enableBtn->setChecked(false);
        return;
    }
    
    if (controlComps->enableBtn->isChecked()) {
        sharedData.enableRequested.store(true);
    } else {
        sharedData.enableRequested.store(false);
        speedComps->input->setValue(0);
        sharedData.targetVelocity.store(0);
        }
    }
}

void MonitorWindow::onModeChanged(int index) {
    if (modeManager) {
        modeManager->onModeChanged(index);
    }
}

void MonitorWindow::onModeConfirmClicked() {
    if (modeManager) {
        modeManager->onModeConfirmClicked();
    }
}

void MonitorWindow::onPPParamsConfirmed() {
    if (modeManager) {
        modeManager->onPPParamsConfirmed();
    }
}

void MonitorWindow::onNetworkChanged(int index) {
    if (networkManager) {
        networkManager->onNetworkChanged(index);
    }
}

void MonitorWindow::onRefreshNetwork() {
    if (networkManager) {
        networkManager->onRefreshNetwork();
    }
}

void MonitorWindow::onConfirmNetwork() {
    if (networkManager) {
        networkManager->onConfirmNetwork();
    }
}

void MonitorWindow::onTargetValueChanged() {
    if (motorController) {
        motorController->onTargetValueChanged();
    }
}

void MonitorWindow::onPVParamsConfirmed() {
    if (modeManager) {
        modeManager->onPVParamsConfirmed();
    }
}

void MonitorWindow::onPTParamsConfirmed() {
    if (modeManager) {
        modeManager->onPTParamsConfirmed();
    }
}

void MonitorWindow::onCSTParamsConfirmed() {
    if (modeManager) {
        modeManager->onCSTParamsConfirmed();
    }
}

void MonitorWindow::onModeToggled(bool checked) {
    showMode = checked;
    if (plotManager) {
    plotComps->plot->graph(3)->setVisible(checked);
    updateYAxisRange();
    plotComps->plot->replot();
    }
}