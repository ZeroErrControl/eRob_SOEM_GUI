#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QStackedWidget>
#include <QTextEdit>
#include "qcustomplot.h"

class ComponentManager {
public:
    // Singleton pattern
    static ComponentManager& getInstance() {
        static ComponentManager instance;
        return instance;
    }

    // 添加全局样式应用函数声明
    void applyGlobalStyle();

    // network interface components
    struct NetworkComponents {
        QComboBox* selector;
        QPushButton* refreshBtn;
        QPushButton* confirmBtn;
        QVBoxLayout* layout;
    };

    // motion mode components
    struct ModeComponents {
        QComboBox* selector;
        QPushButton* confirmBtn;
        QStackedWidget* paramsStack;
        QHBoxLayout* layout;
    };

    // PP mode parameters components
    struct PPComponents {
        QSpinBox* velocityInput;
        QSpinBox* accelInput;
        QSpinBox* decelInput;
        QPushButton* confirmBtn;
        QWidget* panel;
    };

    // speed control components
    struct SpeedComponents {
        QHBoxLayout* layout;
        QSpinBox* input;
        QPushButton* setBtn;
        QWidget* container;
    };

    // enable control components
    struct ControlComponents {
        QPushButton* enableBtn;
        QHBoxLayout* layout;
        QLabel* statusIndicator;  // 添加状态指示器
        QLabel* statusText;       // 添加状态文本
    };

    // plot components
    struct PlotComponents {
        QCustomPlot* plot;
        QPushButton* velocityBtn;
        QPushButton* positionBtn;
        QPushButton* torqueBtn;
        QPushButton* modeBtn;
        QHBoxLayout* controlLayout;
        QHBoxLayout* layout;          // 主布局成员
    };

    // CSV mode parameters components
    struct CSVComponents {
        QSpinBox* velocityInput;
        QSpinBox* accelInput;
        QSpinBox* decelInput;
        QPushButton* confirmBtn;
        QWidget* panel;
    };

    // CSP mode parameters components
    struct CSPComponents {
        QSpinBox* positionInput;
        QSpinBox* velocityInput;
        QPushButton* confirmBtn;
        QWidget* panel;
    };

    // CST mode parameters components
    struct CSTComponents {
        QWidget* panel;
        QSpinBox* maxTorqueInput;    // max torque input
        QSpinBox* torqueSlopeInput;  // torque slope input
        QPushButton* confirmBtn;     // confirm button
    };

    // PV mode parameters components
    struct PVComponents {
        QWidget* panel;
        QSpinBox* accelInput;    // only keep acceleration
        QSpinBox* decelInput;    // and deceleration
        QPushButton* confirmBtn;
    };

    // PT mode parameters components
    struct PTComponents {
        QWidget* panel;
        QSpinBox* maxTorqueInput;     // add max torque input
        QSpinBox* torqueSlopeInput;   // torque slope
        QPushButton* confirmBtn;
    };

    // target value components
    struct TargetComponents {
        QWidget* container;
        QHBoxLayout* layout;
        QSpinBox* input;
        QPushButton* setBtn;
        QLabel* label;
        QLabel* statusIndicator;  // 添加状态指示器
        QLabel* statusText;       // 添加状态文本
    };

    // create components
    NetworkComponents* createNetworkComponents(QWidget* parent);
    ModeComponents* createModeComponents(QWidget* parent);
    PPComponents* createPPComponents(QWidget* parent);
    SpeedComponents* createSpeedComponents(QWidget* parent);
    ControlComponents* createControlComponents(QWidget* parent);
    PlotComponents* createPlotComponents(QWidget* parent);
    CSVComponents* createCSVComponents(QWidget* parent);
    CSPComponents* createCSPComponents(QWidget* parent);
    CSTComponents* createCSTComponents(QWidget* parent);
    PVComponents* createPVComponents(QWidget* parent);
    PTComponents* createPTComponents(QWidget* parent);
    TargetComponents* createTargetComponents(QWidget* parent);

    // update component status
    void updateNetworkStatus(bool enabled);
    void updateModeStatus(bool enabled);
    void updatePPStatus(bool enabled);
    void updateControlStatus(bool enabled);

private:
    ComponentManager() {} // 私有构造函数
    
    // component pointers
    NetworkComponents* networkComps = nullptr;
    ModeComponents* modeComps = nullptr;
    PPComponents* ppComps = nullptr;
    SpeedComponents* speedComps = nullptr;
    ControlComponents* controlComps = nullptr;
    PlotComponents* plotComps = nullptr;
    CSVComponents* csvComps = nullptr;
    CSPComponents* cspComps = nullptr;
    CSTComponents* cstComps = nullptr;
    PVComponents* pvComps = nullptr;
    PTComponents* ptComps = nullptr;
    TargetComponents* targetComps = nullptr;
}; 