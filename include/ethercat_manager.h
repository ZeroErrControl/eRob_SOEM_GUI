#pragma once

#include <memory>
#include "ethercat.h"
#include "pdo_manager.h"
#include <string>
#include <functional>

class EtherCATManager {
public:
    static EtherCATManager& getInstance() {
        static EtherCATManager instance;
        return instance;
    }
    
    // delete copy constructor and assignment operator
    EtherCATManager(const EtherCATManager&) = delete;
    EtherCATManager& operator=(const EtherCATManager&) = delete;
    
    // set destructor to public
    ~EtherCATManager() { cleanup(); }
    
    bool initialize(const std::string& ifname);
    bool sendProcessData();
    int receiveProcessData(int timeout);
    void cleanup();
    bool scanSlaves();
    
    // EtherCAT core functions
    bool configureSlaves();
    bool configurePDOs();
    bool setupDC();
    bool toOperational();
    
    // state management
    bool checkState();
    bool setState(uint16 state);

    // new methods
    bool readObjectDictionary(int slave);
    bool readBasicInfo(int slave);
    bool readPDOConfig(int slave);
    bool readDriveInfo(int slave);

    // communication related methods
    int getExpectedWKC() const { return expectedWKC; }
    int getWorkingCounter() const { return workingCounter; }
    char* getIOmap() { return IOmap; }

    // add DCinfo declaration
    bool DCinfo();

    // modify log callback function type, use std::string instead of QString
    using LogCallback = std::function<void(const std::string&)>;
    void setLogCallback(LogCallback callback) { logCallback = callback; }

    // add get slave count method
    int getSlaveCount() const { return ec_slavecount; }

protected:
    // allow constructor to be inherited
    EtherCATManager() = default;
    
private:
    bool initialized = false;
    char IOmap[4096];
    int expectedWKC = 0;
    volatile int workingCounter = 0;
    std::string interface;

    bool checkInitState();
    bool checkPreOpState();
    bool waitForState(int slave, uint16 state, int timeout = EC_TIMEOUTSTATE * 4);

    LogCallback logCallback;
    void log(const std::string& msg) {
        if (logCallback) logCallback(msg);
    }
}; 