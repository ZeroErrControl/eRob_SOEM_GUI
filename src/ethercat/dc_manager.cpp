// Function: Implement time synchronization and cycle control

#include "dc_manager.h"
#include <cstdio>
#include <unistd.h>

extern ecx_contextt ecx_context;

bool DCManager::configureDC() {
    printf("Configuring DC...\n");
    

        
    for (int i = 1; i <= ec_slavecount; i++) {
        // Configure DC for each slave
        ecx_dcsync0(&ecx_context, i, TRUE, 1000000, 0);  // 500us cycle time
        // Verify configuration
        if (ec_slave[i].hasdc) {
            if (!ec_slave[i].DCactive) {
                printf("Warning: DC not active for slave %d\n", i);
                return false;
            }
            printf("DC configured successfully for slave %d\n", i);
        }
    }
    
    // Configure DC
    ec_configdc();
    
    // Wait for DC configuration to take effect
    osal_usleep(200000);  // 200ms

    return true;
}

bool DCManager::setupDCSync0(int slave, bool active, uint32_t cycleTime, int32_t shiftTime) {
    if (slave < 1 || slave > ec_slavecount) {
        printf("Invalid slave number: %d\n", slave);
        return false;
    }

    // Check if slave supports DC
    if (!ec_slave[slave].hasdc) {
        printf("Slave %d does not support DC\n", slave);
        return false;
    }

    // Configure DC SYNC0
    ecx_dcsync0(&ecx_context, slave, active, cycleTime, shiftTime);
    
    // Verify if configuration was successful
    if (!ec_slave[slave].DCactive) {
        printf("Failed to activate DC for slave %d\n", slave);
        return false;
    }

    return true;
}

void DCManager::printDCStatus() const {
    printf("\nDC Status for all slaves:\n");
    for (int i = 1; i <= ec_slavecount; i++) {
        printf("Slave %d:\n", i);
        printf("  DC Active: %d\n", ec_slave[i].DCactive);
        printf("  DC Supported: %d\n", ec_slave[i].hasdc);
        printf("  DC Cycle Time: %d ns\n", ec_slave[i].DCcycle);
        printf("  DC Shift: %d ns\n", ec_slave[i].DCshift);
    }
}