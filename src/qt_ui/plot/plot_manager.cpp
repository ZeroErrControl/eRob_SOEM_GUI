#include "qt_ui/plot/plot_manager.h"
#include "monitor_window.h"

PlotManager::PlotManager(MonitorWindow* parent, ComponentManager::PlotComponents* plotComps)
    : QObject(parent), mainWindow(parent), plotComps(plotComps), 
      lockView(false), autoYRange(true) {
    
    initializePlot();
    setupInteractions();
}

void PlotManager::initializePlot() {
    // Chart initialization code
    plotComps->plot->addGraph(); // Velocity chart (index 0)
    plotComps->plot->addGraph(); // Position chart (index 1)
    plotComps->plot->addGraph(); // Torque chart (index 2)
    plotComps->plot->addGraph(); // Mode chart (index 3)
    
    // Set chart properties
    plotComps->plot->graph(0)->setPen(QPen(Qt::blue));
    plotComps->plot->graph(1)->setPen(QPen(Qt::red));
    plotComps->plot->graph(2)->setPen(QPen(Qt::green));
    plotComps->plot->graph(3)->setPen(QPen(Qt::magenta));
    
    // Enable chart interaction features
    plotComps->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plotComps->plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    plotComps->plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    
    // Performance optimization settings
    plotComps->plot->setNotAntialiasedElements(QCP::aeAll);
    plotComps->plot->setPlottingHints(QCP::phFastPolylines);
}

void PlotManager::updatePlotData(double key, const monitor::SharedData::DataPoint& point) {
    // Data point update logic
    if (mainWindow->getShowVelocity())
        plotComps->plot->graph(0)->addData(key, point.velocity);
    if (mainWindow->getShowPosition())
        plotComps->plot->graph(1)->addData(key, point.position);
    if (mainWindow->getShowTorque())
        plotComps->plot->graph(2)->addData(key, point.torque);
    
    // Temporarily comment out these two lines until appropriate accessor methods are added
}

void PlotManager::cleanupOldData(double key) {
    // Clean up old data points
    for (int i = 0; i < 4; ++i) {
        plotComps->plot->graph(i)->data()->removeBefore(key - 8.0);
    }
}

void PlotManager::replotWithCurrentSettings(double key) {
    // Redraw chart according to current settings
    if (!lockView) {
        // Maintain current X-axis range width, move to latest data
        double rangeSize = plotComps->plot->xAxis->range().size();
        plotComps->plot->xAxis->setRange(key, rangeSize, Qt::AlignRight);
    }
    
    updateYAxisRange();
    plotComps->plot->replot(QCustomPlot::rpQueuedReplot);
}

void PlotManager::updateYAxisRange(bool forceUpdate) {
    // Y-axis range update logic
    if (!forceUpdate && !autoYRange) return;
    
    // Y-axis range calculation...
}

void PlotManager::setupInteractions() {
    // Set chart interactions
    plotComps->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plotComps->plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    plotComps->plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    
    // Connect signal slots
    connect(plotComps->plot, &QCustomPlot::mousePress, this, [this](QMouseEvent* event) {
        // Mouse press event handling
        if (event->button() == Qt::RightButton) {
            // Right-click menu or other operations
        }
    });
    
    // Add other interaction settings
    plotComps->plot->setContextMenuPolicy(Qt::CustomContextMenu);
}