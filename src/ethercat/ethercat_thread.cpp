/* 
 * This program is an EtherCAT master implementation that initializes and configures EtherCAT slaves,
 * manages their states, and handles real-time data exchange. It includes functions for setting up 
 * PDO mappings, synchronizing time with the distributed clock, and controlling servomotors in 
 * various operational modes. The program also features multi-threading for real-time processing 
 * and monitoring of the EtherCAT network.
 */
//#include <QCoreApplication>


#include <stdio.h>
#include <string.h>
#include "ethercat.h"
#include <iostream>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

#include <sys/time.h>
#include <pthread.h>
#include <math.h>

#include <chrono>
#include <ctime>

#include <iostream>
#include <cstdint>

#include <sched.h>


#include "ethercat_thread.h"

// Add necessary header files
#include "monitor_window.h"  // Include monitor::SharedData definition

// Forward declarations instead of includes if needed
class PDOManager;
class EtherCATManager;
class DCManager;

// External global variable declarations
extern monitor::SharedData sharedData;
extern int dorun;


EtherCATThread::EtherCATThread(QObject *parent)
    : QThread(parent)
    , ecatManager(EtherCATManager::getInstance())
    , dcManager(DCManager::getInstance())
    , running(false)
{
    // Constructor implementation
}

EtherCATThread::~EtherCATThread()
{
        stop();
    wait();
}



bool EtherCATThread::initialize()
{
    // 1. Initialize EtherCAT
    printf("__________STEP 1___________________\n");
    
    return true;
}

void EtherCATThread::stop()
{
    printf("EtherCATThread::stop() called\n");
    
    // Set stop flag
    running = false;
    
    // Set global stop flag
    sharedData.isRunning.store(false);
    dorun = 0;
    
    printf("Stop flags set, waiting for thread to exit...\n");
    
    // Wait for thread to stop
    if (isRunning()) {
        if (!wait(5000)) {  // 5 second timeout
            printf("Warning: EtherCAT thread did not stop within 5 seconds, terminating...\n");
            terminate();
            wait(2000);  // Wait another 2 seconds
        } else {
            printf("EtherCAT thread stopped successfully\n");
        }
    }
}

void EtherCATThread::run() {
    // Set run flag
    running = true;
    
    // Set EtherCAT thread affinity
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);  // EtherCAT main thread uses CPU 2
    CPU_SET(3, &cpuset);  // EtherCAT auxiliary thread uses CPU 3
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    // Set EtherCAT thread priority
    struct sched_param param;
    param.sched_priority = 99;  // Highest real-time priority
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    // Lock memory to prevent page swapping
    mlockall(MCL_CURRENT | MCL_FUTURE);

    printf("Running EtherCAT on CPU cores 2 and 3\n");

    // Run EtherCAT main function
    int result = erob_test();
    
    // Remove re-initialization logic to avoid restarting EtherCAT during shutdown
    if (result == 0) {
        printf("EtherCAT initialization cancelled or completed\n");
    }

    // Set run flag to false
    running = false;
    printf("EtherCAT thread exited\n");
}

// Implement signal-slot functions
void EtherCATThread::newDataReceived(int position, int velocity, short torque) {
    emit dataReceived(position, velocity, torque);
}

void EtherCATThread::motorStatusUpdated(const MotorStatus& status) {
    emit statusUpdated(status);
}

void EtherCATThread::errorOccurred(const QString& error) {
    emit errorSignal(error);  // Use new signal name
}

