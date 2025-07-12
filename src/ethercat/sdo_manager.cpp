#include "sdo_manager.h"

#include <cstdio>
#include <cstring>  //add memcpy header file

bool SDOManager::writeSDO(uint16_t slave, uint16_t index, uint8_t subindex,
                         int size, void* data, const char* description) {
    if (ec_SDOwrite(slave, index, subindex, FALSE, size, data, EC_TIMEOUTRXM) <= 0) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                    "Failed to set %s (index: 0x%04X:%d)", description, index, subindex);
        lastError = error_msg;
        printf("%s\n", lastError.c_str());
        return false;
    }
    return true;
}

bool SDOManager::setPPModeParams(uint16_t slave, const PPParams& params) {
    bool success = true;
    
    // set profile velocity
    if (!writeSDO(slave, 0x6081, 0, sizeof(params.velocity), 
                  (void*)&params.velocity, "profile velocity")) {
        success = false;
    }
    
    // if velocity is set successfully, continue to set acceleration
    if (success && !writeSDO(slave, 0x6083, 0, sizeof(params.acceleration), 
                            (void*)&params.acceleration, "profile acceleration")) {
        success = false;
    }
    
    // if the first two parameters are set successfully, set deceleration
    if (success && !writeSDO(slave, 0x6084, 0, sizeof(params.deceleration), 
                            (void*)&params.deceleration, "profile deceleration")) {
        success = false;
    }
    
    if (success) {
        printf("All parameters of PP mode are set successfully\n");
    }
    
    return success;
}

bool SDOManager::setPVModeParams(uint16_t slave, const PVParams& params) {
    bool success = true;

    // set acceleration
    success &= writeSDO(slave, 0x6083, 0, sizeof(params.acceleration), 
                       (void*)&params.acceleration, "acceleration");
    
    // set deceleration
    success &= writeSDO(slave, 0x6084, 0, sizeof(params.deceleration), 
                       (void*)&params.deceleration, "deceleration");
    
    return success;
}

bool SDOManager::setPTModeParams(uint16_t slave, const PTParams& params) {
    bool success = true;
    // set max torque
    success &= writeSDO(slave, 0x6072, 0, sizeof(params.max_torque), 
                       (void*)&params.max_torque, "max torque");

    // set torque slope
    success &= writeSDO(slave, 0x6087, 0, sizeof(params.torque_slope), 
                       (void*)&params.torque_slope, "torque slope");
    
    return success;
}

bool SDOManager::setCSPModeParams(uint16_t slave, const CSPParams& params) {
    bool success = true;
    
    // set target position
    success &= writeSDO(slave, 0x607A, 0, sizeof(params.position), 
                       (void*)&params.position, "target position");
    
    // set velocity limit
    success &= writeSDO(slave, 0x607F, 0, sizeof(params.velocity), 
                       (void*)&params.velocity, "max profile velocity");
    
    return success;
}

bool SDOManager::setCSVModeParams(uint16_t slave, const CSVParams& params) {
    bool success = true;
    
    // set target velocity
    success &= writeSDO(slave, 0x60FF, 0, sizeof(params.velocity), 
                       (void*)&params.velocity, "target velocity");
    
    // set acceleration
    success &= writeSDO(slave, 0x6083, 0, sizeof(params.acceleration), 
                       (void*)&params.acceleration, "acceleration");
    
    // set deceleration
    success &= writeSDO(slave, 0x6084, 0, sizeof(params.deceleration), 
                       (void*)&params.deceleration, "deceleration");
    
    return success;
}

bool SDOManager::setOperationMode(uint16_t slave, uint8_t mode) {
    return writeSDO(slave, 0x6060, 0, sizeof(mode), 
                   (void*)&mode, "operation mode");
}

bool SDOManager::setCSTParams(const CSTParams& params) {
    bool success = true;
    for (int slave = 1; slave <= ec_slavecount; slave++) {
        success &= setCSTParams(slave, params);
    }
    return success;
}

bool SDOManager::setCSTParams(uint16_t slave, const CSTParams& params) {
    bool success = true;
    
    // set max torque (0x6072)
    success &= writeSDO(slave, 0x6072, 0, sizeof(params.max_torque), 
                       (void*)&params.max_torque, "max torque");
    
    // set torque slope (0x6087)
    success &= writeSDO(slave, 0x6087, 0, sizeof(params.torque_slope), 
                       (void*)&params.torque_slope, "torque slope");
    
    return success;
}