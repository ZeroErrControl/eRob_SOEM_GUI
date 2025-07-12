#pragma once

#include <QThread>
#include <QString>
#include <QMutex>
#include <memory>
#include <thread>
#include <chrono>
#include "ethercat.h"
#include "ethercat_manager.h"
#include "dc_manager.h"
#include "pdo_manager.h"
#include "osal.h"
#include <sys/resource.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

// if needed, add these definitions
#ifdef __linux__
#include <pthread.h>
typedef pthread_t osal_thread_handle_t;
#endif
extern int erob_test();
// define MotorStatus structure
struct MotorStatus {
    bool is_operational;
    uint16_t statusword;
    int32_t actual_position;
    int32_t actual_velocity;
    int16_t actual_torque;
};

// forward declaration of period information structure
struct period_info {
    long long period_ns;
};

class EtherCATThread : public QThread {
    Q_OBJECT

private:
        // define thread function type
    typedef void* (*ThreadFunc)(void*);
    
    static volatile bool s_start_ecatthread;  // add static member declaration
    
    // thread handle
    osal_thread_handle_t thread1;  // real-time thread handle
    osal_thread_handle_t thread2;  // check thread handle

    // thread control variable
    volatile bool running;
    int expectedWKC;
    volatile int wkc;
    int64_t toff;
    int dorun;
    int ctime_thread;

    // use PDO structure defined in PDOManager
    PDOManager::RxPDO rxpdo;
    PDOManager::TxPDO txpdo;

    // thread function
    static ThreadFunc getEcatRTThread() { return &ecatRTThread; }
    static ThreadFunc getEcatCheckThread() { return &ecatCheckThread; }
    
    static void* ecatRTThread(void* arg);
    static void* ecatCheckThread(void* arg);

    // private helper functions
    void setupThreadParameters();
    bool configurePDOMapping();
    bool configureDC();
    bool processData();
    void cleanup();
    bool transitionToOperational();

    QString interfaceName;
    mutable QMutex mutex;
    EtherCATManager& ecatManager;
    DCManager& dcManager;
    std::unique_ptr<PDOManager> pdoManager;

    // add updateMotorStatus declaration
    void updateMotorStatus();

    // add IOmap
    uint8_t IOmap[4096];  // 通常 4KB 足够了

    // add error handling function declaration
    void handleCommunicationError(int& retry_count, const int MAX_RETRY, const int RETRY_DELAY_MS);

    // add private log function
    void logMessage(const QString& msg) {
        emit messageReceived(msg);
    }

    cpu_set_t checkThreadCPUSet;

public:
    explicit EtherCATThread(QObject* parent = nullptr);
    ~EtherCATThread();

    bool initialize();
    void stop();
    void setTargetPosition(int32_t position);

    bool shouldRun() const {
        QMutexLocker locker(&mutex);
        return running;
    }

    // get native thread handle
    pthread_t getNativeHandle() const {
        return pthread_self();
    }

    void setCheckThreadAffinity(const cpu_set_t& cpuset) {
        checkThreadCPUSet = cpuset;
    }

signals:
    void dataReceived(int position, int velocity, short torque);
    void statusUpdated(const MotorStatus& status);
    void errorSignal(const QString& error);  // rename to avoid conflict with error function
    void messageReceived(const QString& msg);
    void motorStateChanged();

public slots:
    void newDataReceived(int position, int velocity, short torque);
    void motorStatusUpdated(const MotorStatus& status);
    void errorOccurred(const QString& error);

protected:
    void run() override;

private:
    // move ec_sync to class internal
    void ec_sync();

}; 

