#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include "component_manager.h"

class MonitorWindowUI {
public:
    struct UIElements {
        QWidget* centralWidget;
        QWidget* leftPanel;
        QWidget* rightPanel;
        QWidget* controlWidget;
        QSplitter* mainSplitter;
        QSplitter* leftSplitter;
        QGroupBox* networkGroupBox;
        QGroupBox* logGroupBox;
        QGroupBox* chartControlBox;
        QTextEdit* logDisplay;
        QCheckBox* lockViewCheckBox;
        QCheckBox* autoYRangeCheckBox;
        QPushButton* resetViewBtn;
    };

    static UIElements createMainLayout(QMainWindow* window, 
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
                                     ComponentManager::CSPComponents* cspComps);

    static void setupChartControls(ComponentManager::PlotComponents* plotComps,
                                 QGroupBox* chartControlBox,
                                 QCheckBox* lockViewCheckBox,
                                 QCheckBox* autoYRangeCheckBox,
                                 QPushButton* resetViewBtn,
                                 QMainWindow* window);

    static void setupModeStack(ComponentManager::ModeComponents* modeComps,
                             ComponentManager::PPComponents* ppComps,
                             ComponentManager::PVComponents* pvComps,
                             ComponentManager::PTComponents* ptComps,
                             ComponentManager::CSTComponents* cstComps,
                             ComponentManager::CSVComponents* csvComps,
                             ComponentManager::CSPComponents* cspComps);

    static void setupPlotGraphs(ComponentManager::PlotComponents* plotComps);
}; 