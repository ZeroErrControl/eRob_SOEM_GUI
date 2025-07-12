#include "component_manager.h"
#include <QLabel>
#include <QGroupBox>
#include <QApplication>
#include <QPalette>
#include <QTabWidget>
#include <QStyleFactory>

// Create a more beautiful color scheme
namespace UIColors {
    const QColor primary(0, 120, 215);     // 主题色，蓝色 primary color, blue
    const QColor secondary(10, 189, 198);  // 次要色，青色 translation: secondary color, cyan
    const QColor success(46, 204, 113);    // 成功色，绿色 translation: success color, green
    const QColor warning(241, 196, 15);    // 警告色，黄色 translation: warning color, yellow
    const QColor danger(231, 76, 60);      // 危险色，红色 translation: danger color, red
    const QColor light(240, 240, 240);     // 浅色，接近白色 translation: light color, near white
    const QColor dark(52, 73, 94);         // 深色，接近黑色 translation: dark color, near black
    const QColor text(44, 62, 80);         // 文本色，深灰色 translation: text color, dark gray
}

// Apply global style
void ComponentManager::applyGlobalStyle() {
    // Set application style
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    
    // Create palette
    QPalette palette;
    palette.setColor(QPalette::Window, UIColors::light);
    palette.setColor(QPalette::WindowText, UIColors::text);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, UIColors::light);
    palette.setColor(QPalette::ToolTipBase, UIColors::dark);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, UIColors::text);
    palette.setColor(QPalette::Button, UIColors::light);
    palette.setColor(QPalette::ButtonText, UIColors::text);
    palette.setColor(QPalette::Link, UIColors::primary);
    palette.setColor(QPalette::Highlight, UIColors::primary);
    palette.setColor(QPalette::HighlightedText, Qt::white);
    
    QApplication::setPalette(palette);
    
    // Set global style sheet
    QString styleSheet = R"(
        QGroupBox {
            border: 1px solid #bbb;
            border-radius: 5px;
            margin-top: 8px;
            padding-top: 8px;
            background-color: rgba(255, 255, 255, 150);
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px;
            background-color: palette(window);
        }
        
        QPushButton {
            border: 1px solid #bbb;
            border-radius: 4px;
            padding: 5px 15px;
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                             stop: 0 #f6f7fa, stop: 1 #dadbde);
        }
        
        QPushButton:pressed {
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                             stop: 0 #dadbde, stop: 1 #f6f7fa);
        }
        
        QPushButton:hover {
            border-color: #7a99c7;
        }
        
        QPushButton:focus {
            border-color: #7a99c7;
            outline: none;
        }
        
        QPushButton[checkable="true"]:checked {
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                             stop: 0 #a8c1e6, stop: 1 #7a99c7);
            color: white;
        }
        
        QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
            padding: 4px;
            border: 1px solid #bbb;
            border-radius: 4px;
            selection-background-color: palette(highlight);
        }
        
        QTabWidget::pane {
            border: 1px solid #bbb;
            border-radius: 4px;
            top: -1px;
        }
        
        QTabBar::tab {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                       stop: 0 #f0f0f0, stop: 1 #e0e0e0);
            border: 1px solid #bbb;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            padding: 5px 10px;
            min-width: 80px;
        }
        
        QTabBar::tab:selected {
            background: white;
            border-bottom-color: white;
        }
        
        QTextEdit {
            border: 1px solid #bbb;
            border-radius: 4px;
        }
    )";
    
    qApp->setStyleSheet(styleSheet);
}

// Create network components, including EtherCAT interface selector and related buttons
ComponentManager::NetworkComponents* ComponentManager::createNetworkComponents(QWidget* parent) {
    if (!networkComps) {
        networkComps = new NetworkComponents;
        
        // Only create content layout, not QGroupBox
        QVBoxLayout* groupLayout = new QVBoxLayout();
        
        // Create interface elements: label, dropdown selector, refresh button and confirm button
        QHBoxLayout* interfaceLayout = new QHBoxLayout();
        QLabel* label = new QLabel("EtherCAT interface:", parent);
        networkComps->selector = new QComboBox(parent);
        networkComps->selector->setMinimumWidth(200);
        
        interfaceLayout->addWidget(label);
        interfaceLayout->addWidget(networkComps->selector, 1);
        
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        networkComps->refreshBtn = new QPushButton("Refresh interface", parent);
        networkComps->confirmBtn = new QPushButton("Confirm connection", parent);
        
        // Set tooltips
        networkComps->selector->setToolTip("Select the network interface to use for EtherCAT communication");
        networkComps->refreshBtn->setToolTip("Refresh available network interface list");
        networkComps->confirmBtn->setToolTip("Confirm and connect to the selected network interface");
        
        buttonLayout->addWidget(networkComps->refreshBtn);
        buttonLayout->addWidget(networkComps->confirmBtn);
        buttonLayout->addStretch();
        
        // Add to layout
        groupLayout->addLayout(interfaceLayout);
        groupLayout->addLayout(buttonLayout);
        
        networkComps->layout = groupLayout;
        networkComps->confirmBtn->setEnabled(false);
    }
    return networkComps;
}

// Create motion mode selection components, including selectors for different motion modes
ComponentManager::ModeComponents* ComponentManager::createModeComponents(QWidget* parent) {
    if (!modeComps) {
        modeComps = new ModeComponents;
        
        // Create group box
        QGroupBox* groupBox = new QGroupBox("2.Motion mode control", parent);
        QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
        
        // Create mode selector
        QHBoxLayout* selectorLayout = new QHBoxLayout();
        QLabel* label = new QLabel("Motion mode:", parent);
        modeComps->selector = new QComboBox(parent);
        modeComps->confirmBtn = new QPushButton("Confirm mode", parent);
        
        // Set tooltips
        modeComps->selector->setToolTip("Select the operation mode of the servo motor");
        modeComps->confirmBtn->setToolTip("Confirm and apply the selected motion mode");
        
        // Add various motion mode options and set icons
        modeComps->selector->addItem("PP mode", 1);
        modeComps->selector->addItem("PV mode", 3);
        modeComps->selector->addItem("PT mode", 4);
        modeComps->selector->addItem("CSP mode", 8);
        modeComps->selector->addItem("CSV mode", 9);
        modeComps->selector->addItem("CST mode", 10);
        
        selectorLayout->addWidget(label);
        selectorLayout->addWidget(modeComps->selector, 1);
        selectorLayout->addWidget(modeComps->confirmBtn);
        
        // Create parameter stack
        modeComps->paramsStack = new QStackedWidget(parent);
        
        // Add to layout
        groupLayout->addLayout(selectorLayout);
        groupLayout->addWidget(modeComps->paramsStack);
        
        // Create overall layout
        modeComps->layout = new QHBoxLayout;
        modeComps->layout->addWidget(groupBox);
    }
    return modeComps;
}

// Create PP mode (Position Profile) parameter setting components
ComponentManager::PPComponents* ComponentManager::createPPComponents(QWidget* parent) {
    if (!ppComps) {
        ppComps = new PPComponents;
        ppComps->panel = new QWidget(parent);
        QVBoxLayout* layout = new QVBoxLayout(ppComps->panel);
        
        // Create group box
        QGroupBox* paramsGroup = new QGroupBox("3.PP mode parameter settings", ppComps->panel);
        QFormLayout* formLayout = new QFormLayout(paramsGroup);
        
        // Create parameter input controls
        ppComps->velocityInput = new QSpinBox(parent);
        ppComps->accelInput = new QSpinBox(parent);
        ppComps->decelInput = new QSpinBox(parent);
        
        // Set range and default values
        ppComps->velocityInput->setRange(0, 50000);
        ppComps->velocityInput->setValue(5000);
        ppComps->velocityInput->setSuffix(" counts/s");
        ppComps->velocityInput->setToolTip("Set the moving speed in the position planning mode");
        
        ppComps->accelInput->setRange(0, 100000);
        ppComps->accelInput->setValue(5000);
        ppComps->accelInput->setSuffix(" counts/s²");
        ppComps->accelInput->setToolTip("Set the acceleration in the position planning mode");
        
        ppComps->decelInput->setRange(0, 100000);
        ppComps->decelInput->setValue(5000);
        ppComps->decelInput->setSuffix(" counts/s²");
        ppComps->decelInput->setToolTip("Set the deceleration in the position planning mode");
        
        // Add to layout
        formLayout->addRow("Planning speed:", ppComps->velocityInput);
        formLayout->addRow("Planning acceleration:", ppComps->accelInput);
        formLayout->addRow("Planning deceleration:", ppComps->decelInput);
        
        // Create confirm button
        ppComps->confirmBtn = new QPushButton("Confirm parameters", parent);
        ppComps->confirmBtn->setToolTip("Confirm and apply PP mode parameter settings");
        
        // 添加到总体布局
        layout->addWidget(paramsGroup);
        layout->addWidget(ppComps->confirmBtn);
        layout->addStretch();
    }
    return ppComps;
}

// Create speed control components for direct velocity setting
ComponentManager::SpeedComponents* ComponentManager::createSpeedComponents(QWidget* parent) {
    auto comps = new SpeedComponents;
    
    // Create layout
    comps->layout = new QHBoxLayout;
    
    // Create label and input
    QLabel* label = new QLabel("Target Velocity:", parent);
    comps->input = new QSpinBox(parent);
    comps->input->setRange(-30000, 30000);
    comps->input->setSingleStep(1000);
    comps->setBtn = new QPushButton("Set", parent);
    
    // Add to layout
    comps->layout->addWidget(label);
    comps->layout->addWidget(comps->input);
    comps->layout->addWidget(comps->setBtn);
    
    // Default disable velocity setting
    comps->input->setEnabled(false);
    comps->setBtn->setEnabled(false);
    
    // Create container widget to control overall visibility
    QWidget* container = new QWidget(parent);
    container->setLayout(comps->layout);
    comps->container = container;
    
    // Default hide
    comps->container->hide();
    
    return comps;
}

// Create motor control components, including enable control button
ComponentManager::ControlComponents* ComponentManager::createControlComponents(QWidget* parent) {
    if (!controlComps) {
        controlComps = new ControlComponents;
        
        // Create group box
        QGroupBox* groupBox = new QGroupBox("4.Motor control", parent);
        QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
        
        // Create enable button
        controlComps->enableBtn = new QPushButton("Enable motor", parent);
        controlComps->enableBtn->setCheckable(true);
        controlComps->enableBtn->setMinimumHeight(40);
        controlComps->enableBtn->setToolTip("Click to enable/disable motor");
        
        // Set enable button style
        controlComps->enableBtn->setStyleSheet(R"(
            QPushButton {
                font-weight: bold;
            }
            QPushButton:checked {
                background-color: #4CAF50;
                color: white;
            }
        )");
        
        // Create status indicator
        QHBoxLayout* statusLayout = new QHBoxLayout();
        QLabel* statusLabel = new QLabel("Motor status:", parent);
        controlComps->statusIndicator = new QLabel(parent);
        controlComps->statusIndicator->setFixedSize(20, 20);
        controlComps->statusIndicator->setStyleSheet("background-color: #f44336; border-radius: 10px;");
        controlComps->statusText = new QLabel("Disabled", parent);
        
        statusLayout->addWidget(statusLabel);
        statusLayout->addWidget(controlComps->statusIndicator);
        statusLayout->addWidget(controlComps->statusText, 1);
        
        // Add to layout
        groupLayout->addWidget(controlComps->enableBtn);
        groupLayout->addLayout(statusLayout);
        
        // Create overall layout
        controlComps->layout = new QHBoxLayout;
        controlComps->layout->addWidget(groupBox);
    }
    return controlComps;
}

// Create plotting components, used to display speed, position and torque curves
ComponentManager::PlotComponents* ComponentManager::createPlotComponents(QWidget* parent) {
    if (!plotComps) {
        plotComps = new PlotComponents;
        
        // Create chart
        plotComps->plot = new QCustomPlot(parent);
        
        // Add all required charts
        plotComps->plot->addGraph(); // Velocity chart (index 0)
        plotComps->plot->addGraph(); // Position chart (index 1)
        plotComps->plot->addGraph(); // Torque chart (index 2)
        plotComps->plot->addGraph(); // Mode chart (index 3)
        
        // Set chart properties
        plotComps->plot->graph(0)->setPen(QPen(Qt::blue));
        plotComps->plot->graph(1)->setPen(QPen(Qt::red));
        plotComps->plot->graph(2)->setPen(QPen(Qt::green));
        plotComps->plot->graph(3)->setPen(QPen(Qt::magenta));
        
        // Create buttons
        plotComps->velocityBtn = new QPushButton("Velocity", parent);
        plotComps->positionBtn = new QPushButton("Position", parent);
        plotComps->torqueBtn = new QPushButton("Torque", parent);
        plotComps->modeBtn = new QPushButton("Mode", parent);
        
        // Set button properties
        plotComps->velocityBtn->setCheckable(true);
        plotComps->positionBtn->setCheckable(true);
        plotComps->torqueBtn->setCheckable(true);
        plotComps->modeBtn->setCheckable(true);
        
        // Set tooltips
        plotComps->velocityBtn->setToolTip("Show/hide velocity curve");
        plotComps->positionBtn->setToolTip("Show/hide position curve");
        plotComps->torqueBtn->setToolTip("Show/hide torque curve");
        plotComps->modeBtn->setToolTip("Show/hide operation mode curve");
        
        // Create button layout
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(plotComps->velocityBtn);
        buttonLayout->addWidget(plotComps->positionBtn);
        buttonLayout->addWidget(plotComps->torqueBtn);
        buttonLayout->addWidget(plotComps->modeBtn);
        buttonLayout->addStretch();
        
        // Create control layout
        plotComps->controlLayout = new QHBoxLayout;
        plotComps->controlLayout->addLayout(buttonLayout);
        plotComps->controlLayout->addWidget(plotComps->plot);
        
        // Create overall layout
        plotComps->layout = new QHBoxLayout;
        plotComps->layout->addLayout(plotComps->controlLayout);
    }
    return plotComps;
}

ComponentManager::CSTComponents* ComponentManager::createCSTComponents(QWidget* parent) {
    if (!cstComps) {
        cstComps = new CSTComponents;
        cstComps->panel = new QWidget(parent);
        QVBoxLayout* layout = new QVBoxLayout(cstComps->panel);
        
        // Maximum torque setting
        QHBoxLayout* maxTorqueLayout = new QHBoxLayout;
        QLabel* maxTorqueLabel = new QLabel("Maximum Torque:", parent);
        cstComps->maxTorqueInput = new QSpinBox(parent);
        cstComps->maxTorqueInput->setRange(0, 100);
        cstComps->maxTorqueInput->setValue(100);  // Default value is 100
        maxTorqueLayout->addWidget(maxTorqueLabel);
        maxTorqueLayout->addWidget(cstComps->maxTorqueInput);
        
        // Torque slope setting
        QHBoxLayout* torqueSlopeLayout = new QHBoxLayout;
        QLabel* torqueSlopeLabel = new QLabel("Torque Slope:", parent);
        cstComps->torqueSlopeInput = new QSpinBox(parent);
        cstComps->torqueSlopeInput->setRange(0, 100);
        cstComps->torqueSlopeInput->setValue(100);  // Default value is 100 
        torqueSlopeLayout->addWidget(torqueSlopeLabel);
        torqueSlopeLayout->addWidget(cstComps->torqueSlopeInput);
        
        // Confirm button
        cstComps->confirmBtn = new QPushButton("Confirm Parameters", parent);
        
        // Add to layout
        layout->addLayout(maxTorqueLayout);
        layout->addLayout(torqueSlopeLayout);
        layout->addWidget(cstComps->confirmBtn);
        layout->addStretch();
    }
    return cstComps;
}

ComponentManager::PVComponents* ComponentManager::createPVComponents(QWidget* parent) {
    if (!pvComps) {
        pvComps = new PVComponents;
        pvComps->panel = new QWidget(parent);
        QVBoxLayout* layout = new QVBoxLayout(pvComps->panel);
        
        // Only keep acceleration and deceleration parameters
        pvComps->accelInput = new QSpinBox(parent);
        pvComps->decelInput = new QSpinBox(parent);
        pvComps->confirmBtn = new QPushButton("Confirm Parameters", parent);
        
        // Set parameter ranges and default values
        pvComps->accelInput->setRange(0, 100000);
        pvComps->decelInput->setRange(0, 100000);
        pvComps->accelInput->setValue(5000);  // Set default acceleration
        pvComps->decelInput->setValue(5000);  // Set default deceleration
        
        layout->addWidget(new QLabel("Profile Acceleration:", parent));
        layout->addWidget(pvComps->accelInput);
        layout->addWidget(new QLabel("Profile Deceleration:", parent));
        layout->addWidget(pvComps->decelInput);
        layout->addWidget(pvComps->confirmBtn);
        layout->addStretch();
    }
    return pvComps;
}

ComponentManager::PTComponents* ComponentManager::createPTComponents(QWidget* parent) {
    if (!ptComps) {
        ptComps = new PTComponents;
        ptComps->panel = new QWidget(parent);
        QVBoxLayout* layout = new QVBoxLayout(ptComps->panel);
        
        // Add maximum torque and torque slope parameters
        ptComps->maxTorqueInput = new QSpinBox(parent);
        ptComps->torqueSlopeInput = new QSpinBox(parent);
        ptComps->confirmBtn = new QPushButton("Confirm Parameters", parent);
        
        // Set parameter ranges and default values
        ptComps->maxTorqueInput->setRange(0, 1000);
        ptComps->maxTorqueInput->setValue(100);  // Default maximum torque is 100
        ptComps->torqueSlopeInput->setRange(0, 10000);
        ptComps->torqueSlopeInput->setValue(100);  // Default torque slope is 100
        
        layout->addWidget(new QLabel("Maximum Torque:", parent));
        layout->addWidget(ptComps->maxTorqueInput);
        layout->addWidget(new QLabel("Torque Slope:", parent));
        layout->addWidget(ptComps->torqueSlopeInput);
        layout->addWidget(ptComps->confirmBtn);
        layout->addStretch();
    }
    return ptComps;
}

ComponentManager::CSVComponents* ComponentManager::createCSVComponents(QWidget* parent) {
    if (!csvComps) {
        csvComps = new CSVComponents;
        csvComps->panel = new QWidget(parent);
        QVBoxLayout* layout = new QVBoxLayout(csvComps->panel);
        
        csvComps->velocityInput = new QSpinBox(parent);
        csvComps->accelInput = new QSpinBox(parent);
        csvComps->decelInput = new QSpinBox(parent);
        csvComps->confirmBtn = new QPushButton("Confirm Parameters", parent);
        
        csvComps->velocityInput->setRange(-100000, 100000);
        csvComps->accelInput->setRange(0, 100000);
        csvComps->decelInput->setRange(0, 100000);
        
        layout->addWidget(new QLabel("Speed:", parent));
        layout->addWidget(csvComps->velocityInput);
        layout->addWidget(new QLabel("Acceleration:", parent));
        layout->addWidget(csvComps->accelInput);
        layout->addWidget(new QLabel("Deceleration:", parent));
        layout->addWidget(csvComps->decelInput);
        layout->addWidget(csvComps->confirmBtn);
        layout->addStretch();
    }
    return csvComps;
}

ComponentManager::CSPComponents* ComponentManager::createCSPComponents(QWidget* parent) {
    if (!cspComps) {
        cspComps = new CSPComponents;
        cspComps->panel = new QWidget(parent);
        QVBoxLayout* layout = new QVBoxLayout(cspComps->panel);
        
        cspComps->positionInput = new QSpinBox(parent);
        cspComps->velocityInput = new QSpinBox(parent);
        cspComps->confirmBtn = new QPushButton("Confirm Parameters", parent);
        
        cspComps->positionInput->setRange(-1000000, 1000000);
        cspComps->velocityInput->setRange(0, 50000);
        
        layout->addWidget(new QLabel("Target Position:", parent));
        layout->addWidget(cspComps->positionInput);
        layout->addWidget(new QLabel("Speed Limit:", parent));
        layout->addWidget(cspComps->velocityInput);
        layout->addWidget(cspComps->confirmBtn);
        layout->addStretch();
    }
    return cspComps;
}

ComponentManager::TargetComponents* ComponentManager::createTargetComponents(QWidget* parent) {
    if (!targetComps) {
        targetComps = new TargetComponents;
        
        // Create container
        targetComps->container = new QWidget(parent);
        
        // Create group box
        QGroupBox* groupBox = new QGroupBox("5.Target value settings", targetComps->container);
        QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
        
        // Create controls
        QHBoxLayout* inputLayout = new QHBoxLayout();
        targetComps->label = new QLabel("Target value:", parent);
        targetComps->input = new QSpinBox(parent);
        targetComps->setBtn = new QPushButton("Set", parent);
        
        // Set style and properties
        targetComps->input->setRange(-1000000, 1000000);
        targetComps->input->setSingleStep(1000);
        targetComps->input->setMinimumWidth(150);
        
        targetComps->setBtn->setMinimumWidth(80);
        
        // Set tooltips
        targetComps->input->setToolTip("Set target value (depending on current mode, it could be position, speed or torque)");
        targetComps->setBtn->setToolTip("Confirm and apply target value settings");
        
        // Layout
        inputLayout->addWidget(targetComps->label);
        inputLayout->addWidget(targetComps->input, 1);
        inputLayout->addWidget(targetComps->setBtn);
        
        groupLayout->addLayout(inputLayout);
        
        // Add feedback display
        QHBoxLayout* feedbackLayout = new QHBoxLayout();
        QLabel* feedbackLabel = new QLabel("Target status:", parent);
        targetComps->statusIndicator = new QLabel(parent);
        targetComps->statusIndicator->setFixedSize(16, 16);
        targetComps->statusIndicator->setStyleSheet("background-color: gray; border-radius: 8px;");
        targetComps->statusText = new QLabel("Not set", parent);
        
        feedbackLayout->addWidget(feedbackLabel);
        feedbackLayout->addWidget(targetComps->statusIndicator);
        feedbackLayout->addWidget(targetComps->statusText, 1);
        
        groupLayout->addLayout(feedbackLayout);
        
        // Main layout
        targetComps->layout = new QHBoxLayout(targetComps->container);
        targetComps->layout->addWidget(groupBox);
        
        // Default disabled (but don't hide, unless you have special UI requirements)
        targetComps->input->setEnabled(false);
        targetComps->setBtn->setEnabled(false);
        // targetComps->container->hide();  // Comment out this line, unless you have special UI logic
    }
    return targetComps;
}

// Update network component status (enable/disable)
void ComponentManager::updateNetworkStatus(bool enabled) {
    if (networkComps) {
        networkComps->selector->setEnabled(enabled);
        networkComps->refreshBtn->setEnabled(enabled);
        networkComps->confirmBtn->setEnabled(enabled);
    }
}

// ... Implement other status update functions ... 