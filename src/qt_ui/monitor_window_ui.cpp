#include "monitor_window_ui.h"
#include <QDateTime>
#include <QGuiApplication>
#include <QScreen>
#include <QStatusBar>
#include <QFrame>
#include <QStyle>

MonitorWindowUI::UIElements MonitorWindowUI::createMainLayout(
    QMainWindow* window,
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
    ComponentManager::CSPComponents* cspComps) {
    
    UIElements elements;
    
    // Create main layout
    elements.centralWidget = new QWidget(window);
    QHBoxLayout* horizontalLayout = new QHBoxLayout(elements.centralWidget);
    
    // Create left panel
    elements.leftPanel = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(elements.leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create network settings group
    elements.networkGroupBox = new QGroupBox("1.Network Settings");
    QVBoxLayout* networkLayout = new QVBoxLayout(elements.networkGroupBox);
    networkLayout->addLayout(networkComps->layout);
    leftLayout->addWidget(elements.networkGroupBox);
    
    // Create control panel container
    elements.controlWidget = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(elements.controlWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Mode selection and parameter settings at the top
    mainLayout->addLayout(modeComps->layout);
    mainLayout->addWidget(modeComps->paramsStack);
    mainLayout->addWidget(speedComps->container);
    
    // Motor enable control
    mainLayout->addLayout(controlComps->layout);
    
    // Target value settings
    mainLayout->addWidget(targetComps->container);
    
    // Separator line
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);
    
    // Create system log area
    elements.logGroupBox = new QGroupBox("System Log");
    QVBoxLayout* logLayout = new QVBoxLayout(elements.logGroupBox);
    logLayout->setContentsMargins(5, 15, 5, 5);
    elements.logDisplay = new QTextEdit(window);
    elements.logDisplay->setReadOnly(true);
    elements.logDisplay->document()->setDocumentMargin(5);
    logLayout->addWidget(elements.logDisplay);
    
    // Use vertical splitter
    elements.leftSplitter = new QSplitter(Qt::Vertical);
    elements.leftSplitter->addWidget(elements.controlWidget);
    elements.leftSplitter->addWidget(elements.logGroupBox);
    elements.leftSplitter->setStretchFactor(0, 2);
    elements.leftSplitter->setStretchFactor(1, 3);
    leftLayout->addWidget(elements.leftSplitter);
    
    // Create right monitoring panel
    elements.rightPanel = new QWidget;
    QVBoxLayout* rightLayout = new QVBoxLayout(elements.rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create chart control options container
    elements.chartControlBox = new QGroupBox("Data Display Options");
    QVBoxLayout* chartControlLayout = new QVBoxLayout(elements.chartControlBox);
    
    // Add control options
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(plotComps->velocityBtn);
    buttonLayout->addWidget(plotComps->positionBtn);
    buttonLayout->addWidget(plotComps->torqueBtn);
    buttonLayout->addWidget(plotComps->modeBtn);
    
    // Time range selector
    // if (plotComps->timeRangeSelector) {
    //     QHBoxLayout* timeLayout = new QHBoxLayout();
    //     timeLayout->addWidget(new QLabel("Time Range:"));
    //     timeLayout->addWidget(plotComps->timeRangeSelector);
    //     chartControlLayout->addLayout(timeLayout);
    // }
    
    chartControlLayout->addLayout(buttonLayout);
    
    // Add view control widgets
    QHBoxLayout* viewControlLayout = new QHBoxLayout();
    
    // View lock option
    elements.lockViewCheckBox = new QCheckBox("Lock View");
    elements.lockViewCheckBox->setChecked(false);
    elements.lockViewCheckBox->setToolTip("When enabled, locks the current view range.\n"
                                       "When disabled, the chart will scroll with the latest data,\n"
                                       "but will maintain your zoom level.");
    
    // Reset view button
    elements.resetViewBtn = new QPushButton("Reset View");
    elements.resetViewBtn->setToolTip("Reset chart view and enable auto-scroll function.");
    
    // Y-axis auto range option
    elements.autoYRangeCheckBox = new QCheckBox("Y-axis Auto Range");
    elements.autoYRangeCheckBox->setChecked(true);
    elements.autoYRangeCheckBox->setToolTip("When enabled, Y-axis range automatically adapts to all visible data.\n"
                                         "When disabled, you can manually zoom the Y-axis to view details.");
    
    viewControlLayout->addWidget(elements.lockViewCheckBox);
    viewControlLayout->addWidget(elements.resetViewBtn);
    viewControlLayout->addWidget(elements.autoYRangeCheckBox);
    
    chartControlLayout->addLayout(viewControlLayout);
    
    // Add control options to right panel top
    rightLayout->addWidget(elements.chartControlBox);
    
    // Add chart to right panel
    rightLayout->addWidget(plotComps->plot, 1);
    
    // Create left-right splitter
    elements.mainSplitter = new QSplitter(Qt::Horizontal);
    elements.mainSplitter->addWidget(elements.leftPanel);
    elements.mainSplitter->addWidget(elements.rightPanel);
    elements.mainSplitter->setStretchFactor(0, 1);
    elements.mainSplitter->setStretchFactor(1, 2);
    
    // Add main splitter to layout
    horizontalLayout->addWidget(elements.mainSplitter);
    window->setCentralWidget(elements.centralWidget);
    
    // Setup mode panel stack
    setupModeStack(modeComps, ppComps, pvComps, ptComps, cstComps, csvComps, cspComps);

    // Enable motion mode dropdown (temporary fix, production environment suggests enabling after network initialization success)
    modeComps->selector->setEnabled(true);

    // Setup chart
    setupPlotGraphs(plotComps);
    
    return elements;
}

void MonitorWindowUI::setupChartControls(ComponentManager::PlotComponents* plotComps,
                                       QGroupBox* chartControlBox,
                                       QCheckBox* lockViewCheckBox,
                                       QCheckBox* autoYRangeCheckBox,
                                       QPushButton* resetViewBtn,
                                       QMainWindow* window) {
    
    // Enable chart interaction features
    plotComps->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plotComps->plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    plotComps->plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    
    // Optimize performance
    plotComps->plot->setNotAntialiasedElements(QCP::aeAll);
    plotComps->plot->setPlottingHints(QCP::phFastPolylines);
}

void MonitorWindowUI::setupModeStack(ComponentManager::ModeComponents* modeComps,
                                   ComponentManager::PPComponents* ppComps,
                                   ComponentManager::PVComponents* pvComps,
                                   ComponentManager::PTComponents* ptComps,
                                   ComponentManager::CSTComponents* cstComps,
                                   ComponentManager::CSVComponents* csvComps,
                                   ComponentManager::CSPComponents* cspComps) {
    
    // Add all mode panels to stack
    modeComps->paramsStack->addWidget(csvComps->panel);
    modeComps->paramsStack->addWidget(cspComps->panel);
    modeComps->paramsStack->addWidget(cstComps->panel);
    modeComps->paramsStack->addWidget(ppComps->panel);
    modeComps->paramsStack->addWidget(pvComps->panel);
    modeComps->paramsStack->addWidget(ptComps->panel);
    
    // Set initial state
    modeComps->paramsStack->setCurrentWidget(ppComps->panel);
    modeComps->selector->setEnabled(false);
}

void MonitorWindowUI::setupPlotGraphs(ComponentManager::PlotComponents* plotComps) {
    // Ensure enough graphs are added
    plotComps->plot->addGraph(); // Velocity chart (index 0)
    plotComps->plot->addGraph(); // Position chart (index 1)
    plotComps->plot->addGraph(); // Torque chart (index 2)
    plotComps->plot->addGraph(); // Mode chart (index 3)
    
    // Set chart properties
    plotComps->plot->graph(0)->setPen(QPen(Qt::blue));
    plotComps->plot->graph(1)->setPen(QPen(Qt::red));
    plotComps->plot->graph(2)->setPen(QPen(Qt::green));
    plotComps->plot->graph(3)->setPen(QPen(Qt::magenta));
    
    // Set mode curve style
    plotComps->plot->graph(3)->setPen(QPen(Qt::yellow, 2));
    plotComps->plot->graph(3)->setName("Operation Mode");
} 