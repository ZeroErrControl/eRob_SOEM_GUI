#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <atomic>
#include "pdo_manager.h"
#include "component_manager.h"
#include "ethercat_thread.h"

// External variable declarations
extern int dorun;
extern PDOManager::TxPDO txpdo;

namespace monitor {
    struct SharedData {
        static const int BUFFER_SIZE = 10000;  // buffer size
        struct DataPoint {
            double time;
            double velocity;
            double position;
            double torque;
            double mode;  // 添加模式字段
        };
        
        // PP mode parameter structure
        struct PPParams {
            int32_t velocity;
            int32_t acceleration;
            int32_t deceleration;
        };
        
        // PV mode parameter structure
        struct PVParams {
            int32_t acceleration;
            int32_t deceleration;
        };
        
        // PT mode parameter structure
        struct PTParams {
            int32_t max_torque;    // Added max torque field
            int32_t torque_slope;
        };
        
        // CST mode parameter structure
        struct CSTParams {
            int32_t max_torque;
            int32_t torque_slope;
        };
        
        // Lock-free circular buffer
        DataPoint buffer[BUFFER_SIZE];
        std::atomic<int> writeIndex{0};  // Write position
        std::atomic<int> readIndex{0};   // Read position
        std::atomic<bool> isRunning{true};
        std::atomic<bool> motorEnabled{false};
        std::atomic<int> targetVelocity{0};  // Target speed
        std::atomic<int32_t> targetPosition{0};
        std::atomic<int16_t> targetTorque{0};  // Added target torque
        std::atomic<bool> enableRequested{false};  // Enable request flag
        std::atomic<uint8_t> operationMode{9};  // Default CSV mode
        std::atomic<bool> modeChangeRequested{false};
        std::atomic<bool> modeConfirmed{false};  // Mode confirmation flag
        
        // PP mode related fields
        PPParams ppParams;
        std::atomic<bool> ppParamsConfirmed{false};
        std::atomic<bool> ppParamsNeedUpdate{false};
        
        // PV mode related fields
        PVParams pvParams;
        std::atomic<bool> pvParamsConfirmed{false};
        
        // PT mode related fields
        PTParams ptParams;
        std::atomic<bool> ptParamsConfirmed{false};
        
        // CST mode related fields
        CSTParams cstParams;
        std::atomic<bool> cstParamsConfirmed{false};
        
        std::string selectedInterface;  // Selected network interface
        std::atomic<bool> interfaceConfirmed{false};  // Interface confirmation flag
        std::atomic<bool> pvNewTarget{false};  // PV mode new target speed flag
        int32_t cspMaxVelocity = 10000; // 新增：CSP模式最大速度
    };
}

// 在 MonitorWindow 类定义前添加枚举
enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    SUCCESS
};

// 前向声明
class PlotManager;
class MotorController;
class ModeManager;
class NetworkManager;
class LogManager;
class MonitorWindowEvents;
class MonitorWindowData;

class MonitorWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MonitorWindow(monitor::SharedData& data, QWidget *parent = nullptr);
    void stopUpdates() { running = false; }
    
    // 公共接口方法
    void appendLog(const QString& message, LogLevel level = LogLevel::INFO);
    
    // 获取组件的方法
    ComponentManager::ControlComponents* getControlComponents() { return controlComps; }
    ComponentManager::TargetComponents* getTargetComponents() { return targetComps; }

    // 用于访问类的状态
    bool getShowVelocity() const { return showVelocity; }
    bool getShowPosition() const { return showPosition; }
    bool getShowTorque() const { return showTorque; }
    bool getShowMode() const { return showMode; }

    // 获取 SharedData 引用
    monitor::SharedData& getSharedData() { return sharedData; }

public slots:
    void updatePlot();
    void onVelocityToggled(bool checked);
    void onPositionToggled(bool checked);
    void onTorqueToggled(bool checked);
    void onSpeedInputChanged();
    void onEnableButtonClicked();
    void onModeChanged(int index);
    void onModeConfirmClicked();
    void onPPParamsConfirmed();
    void onNetworkChanged(int index);
    void onRefreshNetwork();
    void onConfirmNetwork();
    void onTargetValueChanged();
    void onPVParamsConfirmed();
    void onPTParamsConfirmed();
    void onCSTParamsConfirmed();
    void onModeToggled(bool checked);

signals:
    void windowClosed();  

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    // 组件管理器指针
    ComponentManager::NetworkComponents* networkComps;
    ComponentManager::ModeComponents* modeComps;
    ComponentManager::PPComponents* ppComps;
    ComponentManager::SpeedComponents* speedComps;
    ComponentManager::ControlComponents* controlComps;
    ComponentManager::PlotComponents* plotComps;
    ComponentManager::CSVComponents* csvComps;
    ComponentManager::CSPComponents* cspComps;
    ComponentManager::CSTComponents* cstComps;
    ComponentManager::PVComponents* pvComps;
    ComponentManager::PTComponents* ptComps;
    ComponentManager::TargetComponents* targetComps;
    
    // 管理器指针
    MonitorWindowEvents* eventsHandler;
    MonitorWindowData* dataManager;
    PlotManager* plotManager;
    MotorController* motorController;
    ModeManager* modeManager;
    NetworkManager* networkManager;
    LogManager* logManager;
    
    // EtherCAT线程
    EtherCATThread* ethercat_thread;
    
    // 成员变量
    monitor::SharedData& sharedData;
    bool running;
    bool showVelocity, showPosition, showTorque, showMode;
    bool lockView, autoYRange;
    QDateTime startTime;
    
    // UI元素
    QTextEdit* logDisplay;
    QCheckBox* lockViewCheckBox;
    QCheckBox* autoYRangeCheckBox;
    QPushButton* resetViewBtn;
    
    // 私有方法
    void connectSignals();
    void startDataTimer();
    void startStateCheckTimer();
    void checkMotorStateChange();

    // 代理方法 - 转发到适当的管理器
    void updateYAxisRange(bool forceUpdate = false);
    void refreshNetworkInterfaces();
    void updateModePanel(int mode);
    void updatePositionStatus(int32_t currentPosition, int32_t targetPosition);
    void updateEnableButtonState();
    void updateNetworkStatusIndicator(bool connected);
    void updateModeStatusIndicator();
};