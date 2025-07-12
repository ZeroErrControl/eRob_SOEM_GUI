#include "monitor_window_data.h"
#include <QDateTime>
#include <algorithm>
#include "qcustomplot.h"

MonitorWindowData::MonitorWindowData(QObject* parent,
                                   monitor::SharedData& sharedData,
                                   ComponentManager::PlotComponents* plotComps,
                                   QDateTime startTime)
    : QObject(parent), sharedData(sharedData), plotComps(plotComps), startTime(startTime) {
    dataTimer = nullptr;
}

void MonitorWindowData::startDataTimer() {
    if (!dataTimer) {
        dataTimer = new QTimer(this);
        connect(dataTimer, &QTimer::timeout, this, &MonitorWindowData::updatePlot);
        dataTimer->start(UPDATE_INTERVAL);
        
        // Get main window pointer to record logs
        MonitorWindow* mainWindow = qobject_cast<MonitorWindow*>(parent());
        if (mainWindow) {
            mainWindow->appendLog("Data timer started", LogLevel::SUCCESS);
            mainWindow->appendLog(QString("Data update interval: %1ms").arg(UPDATE_INTERVAL), LogLevel::INFO);
        }
    }
}

void MonitorWindowData::stopDataTimer() {
    if (dataTimer) {
        dataTimer->stop();
        dataTimer->deleteLater();
        dataTimer = nullptr;
        
        // Get main window pointer to record logs
        MonitorWindow* mainWindow = qobject_cast<MonitorWindow*>(parent());
        if (mainWindow) {
            mainWindow->appendLog("Data timer stopped", LogLevel::INFO);
        }
    }
}

void MonitorWindowData::updatePlot() {
    static int updateCounter = 0;
    static int cleanupCounter = 0;
    static int dataUpdateCount = 0;
    
    // Current timestamp
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0 - startTime.toMSecsSinceEpoch() / 1000.0;
    
    // Get latest written data and update
    int writeIdx = sharedData.writeIndex.load();
    int pointsToProcess = std::min(10, (writeIdx - sharedData.readIndex.load() + monitor::SharedData::BUFFER_SIZE) % monitor::SharedData::BUFFER_SIZE);
    
    if (pointsToProcess > 0) {
        dataUpdateCount += pointsToProcess;
        
        for (int i = 0; i < pointsToProcess; i++) {
            int idx = (sharedData.readIndex.load() + i) % monitor::SharedData::BUFFER_SIZE;
            monitor::SharedData::DataPoint& point = sharedData.buffer[idx];
            
            // Update chart data
            plotComps->plot->graph(0)->addData(key - (pointsToProcess-i)*0.01, point.velocity);
            plotComps->plot->graph(1)->addData(key - (pointsToProcess-i)*0.01, point.position);
            plotComps->plot->graph(2)->addData(key - (pointsToProcess-i)*0.01, point.torque);
            plotComps->plot->graph(3)->addData(key - (pointsToProcess-i)*0.01, point.mode);
        }
        
        // Update read index
        sharedData.readIndex.store((sharedData.readIndex.load() + pointsToProcess) % monitor::SharedData::BUFFER_SIZE);
        
        // Log every 1000 data points
        if (dataUpdateCount >= 1000) {
            MonitorWindow* mainWindow = qobject_cast<MonitorWindow*>(parent());

            /*
                        if (mainWindow) {
                mainWindow->appendLog(QString("Processed %1 data points").arg(dataUpdateCount), LogLevel::INFO);
            }
            */

            dataUpdateCount = 0;
        }
    }
    
    // Periodically clean up old data points
    cleanupCounter++;
    if (cleanupCounter >= CLEANUP_INTERVAL) {
        cleanupOldData(key);
        cleanupCounter = 0;
        
        // Record cleanup operation
        MonitorWindow* mainWindow = qobject_cast<MonitorWindow*>(parent());
        /*
                if (mainWindow) {
            mainWindow->appendLog("Chart data cleanup completed", LogLevel::INFO);
        }
        */

    }
    
    // Redraw chart
    updateCounter++;
    if (updateCounter >= REPLOT_INTERVAL) {
        updateXAxisRange(key);
        updateYAxisRange();
        plotComps->plot->replot();
        updateCounter = 0;
    }
}

void MonitorWindowData::cleanupOldData(double currentTime) {
    // Only remove data older than 10 seconds, compatible with QCustomPlot 1.x/2.x
    for (int i = 0; i < 4; i++) {
        auto graph = plotComps->plot->graph(i);
        if (graph->dataCount() > MAX_POINTS) {
#if QCP_VERSION >= 0x020000
            graph->data()->removeBefore(currentTime - 10.0);
#else
            graph->data()->removeBefore(currentTime - 10.0);
#endif
        }
    }
}

void MonitorWindowData::updateYAxisRange(bool forceUpdate) {
    if (autoYRange || forceUpdate) {
        plotComps->plot->rescaleAxes();
        
        // Record Y-axis range update
        MonitorWindow* mainWindow = qobject_cast<MonitorWindow*>(parent());
        if (mainWindow && forceUpdate) {
            mainWindow->appendLog("Y-axis range updated", LogLevel::INFO);
        }
    }
}

void MonitorWindowData::updateXAxisRange(double currentTime) {
    if (!lockView) {
        plotComps->plot->xAxis->setRange(currentTime, 10, Qt::AlignRight);
    }
}

// Set view control state
void MonitorWindowData::setLockView(bool locked) {
    lockView = locked;
    
    // Record view lock state change
    MonitorWindow* mainWindow = qobject_cast<MonitorWindow*>(parent());
    if (mainWindow) {
        if (locked) {
            mainWindow->appendLog("Chart view locked", LogLevel::INFO);
        } else {
            mainWindow->appendLog("Chart view unlocked", LogLevel::INFO);
        }
    }
}

void MonitorWindowData::setAutoYRange(bool autoRange) {
    autoYRange = autoRange;
    
    // Record auto Y-axis range setting change
    MonitorWindow* mainWindow = qobject_cast<MonitorWindow*>(parent());
    if (mainWindow) {
        if (autoRange) {
            mainWindow->appendLog("Y-axis auto range enabled", LogLevel::INFO);
        } else {
            mainWindow->appendLog("Y-axis auto range disabled", LogLevel::INFO);
        }
    }
} 