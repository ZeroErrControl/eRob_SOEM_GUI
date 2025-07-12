// Real-time monitoring of slave status
// Implement state recovery mechanism

#include "ethercat_manager.h"

#include <cstdio>

bool EtherCATManager::initialize(const std::string& ifname) {
    log("__________STEP 1___________________");
    log("Initializing EtherCAT...");
    
    if (ec_init(ifname.c_str()) <= 0) {
        log("Error: Could not initialize EtherCAT master!");
        log("No socket connection on Ethernet port. Execute as root.");
        log("___________________________________________");
        return false;
    }
    log("EtherCAT master initialized successfully.");
    log("___________________________________________");

    // Search for EtherCAT slaves on the network
    if (ec_config_init(FALSE) <= 0) {
        log("Error: Cannot find EtherCAT slaves!");
        log("___________________________________________");
        ec_close(); // Close the EtherCAT connection
        return false;
    }
    log(std::to_string(ec_slavecount) + " slaves found and configured.");
    log("___________________________________________");
    
    initialized = true; // <--- Ensure initialization is set after success
    return true;
}

bool EtherCATManager::sendProcessData() {
    workingCounter = ec_send_processdata();
    return workingCounter > 0;
}

int EtherCATManager::receiveProcessData(int timeout) {
    workingCounter = ec_receive_processdata(timeout);
    return workingCounter;
}

void EtherCATManager::cleanup() {
    if (!initialized) return; // Prevent cleanup when not initialized
    ec_slave[0].state = EC_STATE_INIT;
    ec_writestate(0);
    ec_close();
    initialized = false;
}

bool EtherCATManager::configureSlaves() {
    log("Configuring slaves...");
    
    if (!initialized) return false;
    
    if (ec_config_init(FALSE) <= 0) {
        return false;
    }
    
    // Configure PDO mapping
    configurePDOs();
    
    bool success = true;
    
    if (success) {
        log("Slaves configured successfully");
        return true;
    } else {
        log("Failed to configure slaves");
        return false;
    }
    return true;
}

bool EtherCATManager::configurePDOs() {
    for (int slave = 1; slave <= ec_slavecount; slave++) {
        if (!PDOManager::configurePDOs(slave)) {
            return false;
        }
    }
    
    // Configure PDO mapping to IOmap
    ec_config_map(&IOmap);
    
    // Calculate expected working counter value
    expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
    printf("Expected working counter: %d\n", expectedWKC);

    return true;
}

bool EtherCATManager::checkState() {
    ec_readstate();
    printf("\nChecking slave states...\n");
    
    for (int i = 1; i <= ec_slavecount; i++) {
        printf("Slave %d - State: 0x%02x, AL Status: 0x%04x - %s\n",
               i, ec_slave[i].state, ec_slave[i].ALstatuscode,
               ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
        
        if (ec_slave[i].state != EC_STATE_PRE_OP) {
            printf("Requesting INIT state for slave %d\n", i);
            ec_slave[i].state = EC_STATE_INIT;
            ec_writestate(i);
            
            if (!waitForState(i, EC_STATE_INIT)) {
                printf("Failed to reach INIT state for slave %d\n", i);
                return false;
            }
        }
    }

    // Set all slaves to PRE-OP state
    ec_slave[0].state = EC_STATE_PRE_OP;
    ec_writestate(0);
    
    if (!waitForState(0, EC_STATE_PRE_OP)) {
        printf("Failed to reach PRE-OP state for all slaves\n");
        return false;
    }

    printf("All slaves successfully reached PRE-OP state\n");
    return true;
}

bool EtherCATManager::setState(uint16 state) {
    ec_slave[0].state = state;
    ec_writestate(0);
    return waitForState(0, state);
}

bool EtherCATManager::waitForState(int slave, uint16 state, int timeout) {
    int retries = timeout;
    while (retries--) {
        ec_readstate();
        if (ec_slave[slave].state == state) {
            return true;
        }
        if (ec_slave[slave].state == (EC_STATE_ERROR)) {
            printf("Slave %d is in ERROR state, AL Status: 0x%04x - %s\n",
                   slave, ec_slave[slave].ALstatuscode,
                   ec_ALstatuscode2string(ec_slave[slave].ALstatuscode));
            return false;
        }
        osal_usleep(100000); // 100ms
    }
    return false;
}

bool EtherCATManager::DCinfo()
{
      ec_readstate(); // Read the state of all slaves
    for(int i = 1; i <= ec_slavecount; i++) {
        printf("Slave %d\n", i);
        printf("  State: %02x\n", ec_slave[i].state); // Print the state of the slave
        printf("  ALStatusCode: %04x\n", ec_slave[i].ALstatuscode); // Print the AL status code
        printf("  Delay: %d\n", ec_slave[i].pdelay); // Print the delay of the slave
        printf("  Has DC: %d\n", ec_slave[i].hasdc); // Check if the slave supports Distributed Clock
        printf("  DC Active: %d\n", ec_slave[i].DCactive); // Check if DC is active for the slave
        printf("  DC supported: %d\n", ec_slave[i].hasdc); // Print if DC is supported
    }

    // Read DC synchronization configuration using the correct parameters
    for(int i = 1; i <= ec_slavecount; i++) {
        uint16_t dcControl = 0; // Variable to hold DC control configuration
        int32_t cycleTime = 0; // Variable to hold cycle time
        int32_t shiftTime = 0; // Variable to hold shift time
        int size; // Variable to hold size for reading

        // Read DC synchronization configuration, adding the correct size parameter
        size = sizeof(dcControl);
        if (ec_SDOread(i, 0x1C32, 0x01, FALSE, &size, &dcControl, EC_TIMEOUTSAFE) > 0) {
            printf("Slave %d DC Configuration:\n", i);
            printf("  DC Control: 0x%04x\n", dcControl); // Print the DC control configuration
            
            size = sizeof(cycleTime);
            if (ec_SDOread(i, 0x1C32, 0x02, FALSE, &size, &cycleTime, EC_TIMEOUTSAFE) > 0) {
                printf("  Cycle Time: %d ns\n", cycleTime); // Print the cycle time
            }

        }
    }

    return true;
}

bool EtherCATManager::scanSlaves() {
    log("Scanning for slaves...");
    
    if (ec_config_init(FALSE) > 0) {
        log("Found " + std::to_string(ec_slavecount) + " slaves");
        
        // Print detailed slave information
        for (int i = 1; i <= ec_slavecount; i++) {
            std::string slaveInfo = "Slave " + std::to_string(i) + ":\n";
            slaveInfo += "  Name: " + std::string(ec_slave[i].name) + "\n";
            slaveInfo += "  Type: 0x" + std::to_string(ec_slave[i].eep_id) + "\n";
            slaveInfo += "  State: " + std::to_string(ec_slave[i].state);
            log(slaveInfo);
        }
        
        return true;
    } else {
        log("No slaves found");
        return false;
    }
}

bool EtherCATManager::readObjectDictionary(int slave) {
    printf("\nReading Object Dictionary for Slave %d:\n", slave);
    
    if (!readBasicInfo(slave)) {
        printf("Failed to read basic info\n");
        return false;
    }

    if (!readPDOConfig(slave)) {
        printf("Failed to read PDO configuration\n");
        return false;
    }

    if (!readDriveInfo(slave)) {
        printf("Failed to read drive info\n");
        return false;
    }

    return true;
}

bool EtherCATManager::readBasicInfo(int slave) {
    int size;
    
    // Read Device Type (0x1000)
    uint32_t deviceType;
    size = sizeof(deviceType);
    if (ec_SDOread(slave, 0x1000, 0, FALSE, &size, &deviceType, EC_TIMEOUTSAFE) > 0) {
        printf("Device Type (0x1000): 0x%08X\n", deviceType);
    }

    // Read Error Register (0x1001)
    uint8_t errorRegister;
    size = sizeof(errorRegister);
    if (ec_SDOread(slave, 0x1001, 0, FALSE, &size, &errorRegister, EC_TIMEOUTSAFE) > 0) {
        printf("Error Register (0x1001): 0x%02X\n", errorRegister);
    }

    // Read Manufacturer Device Name (0x1008)
    char deviceName[100];
    size = sizeof(deviceName);
    if (ec_SDOread(slave, 0x1008, 0, FALSE, &size, &deviceName, EC_TIMEOUTSAFE) > 0) {
        printf("Device Name (0x1008): %s\n", deviceName);
    }

    return true;
}

bool EtherCATManager::readPDOConfig(int slave) {
    int size;
    uint8_t pdoCount;
    
    // Read RxPDO mapping
    size = sizeof(pdoCount);
    if (ec_SDOread(slave, 0x1C12, 0, FALSE, &size, &pdoCount, EC_TIMEOUTSAFE) > 0) {
        printf("RxPDO Mapping Count: %d\n", pdoCount);
        for (int i = 1; i <= pdoCount; i++) {
            uint16_t pdoAssign;
            size = sizeof(pdoAssign);
            if (ec_SDOread(slave, 0x1C12, i, FALSE, &size, &pdoAssign, EC_TIMEOUTSAFE) > 0) {
                printf("  RxPDO %d Assignment: 0x%04X\n", i, pdoAssign);
            }
        }
    }

    // Read TxPDO mapping
    if (ec_SDOread(slave, 0x1C13, 0, FALSE, &size, &pdoCount, EC_TIMEOUTSAFE) > 0) {
        printf("TxPDO Mapping Count: %d\n", pdoCount);
        for (int i = 1; i <= pdoCount; i++) {
            uint16_t pdoAssign;
            size = sizeof(pdoAssign);
            if (ec_SDOread(slave, 0x1C13, i, FALSE, &size, &pdoAssign, EC_TIMEOUTSAFE) > 0) {
                printf("  TxPDO %d Assignment: 0x%04X\n", i, pdoAssign);
            }
        }
    }

    return true;
}

bool EtherCATManager::readDriveInfo(int slave) {
    int size;
    
    // Read Supported Operation Modes (0x6502)
    int32_t supportedModes;
    size = sizeof(supportedModes);
    if (ec_SDOread(slave, 0x6502, 0, FALSE, &size, &supportedModes, EC_TIMEOUTSAFE) > 0) {
        printf("Supported Drive Modes (0x6502): 0x%08X\n", supportedModes);
    }

    return true;
}

// Implement other methods... 