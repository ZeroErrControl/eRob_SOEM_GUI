#pragma once

#include "ethercat.h"
#include <string>

class SDOManager {
public:
    // Singleton pattern
    static SDOManager& getInstance() {
        static SDOManager instance;
        return instance;
    }

    // PP mode parameters structure
    struct PPParams {
        int32_t velocity;
        int32_t acceleration;
        int32_t deceleration;
    };

    // PV mode parameters structure
    struct PVParams {
        int32_t acceleration;
        int32_t deceleration;
    };

    // PT mode parameters structure
    struct PTParams {
        int32_t max_torque;
        int32_t torque_slope;
    };

    // CSP mode parameters structure
    struct CSPParams {
        int32_t position;
        int32_t velocity;
    };

    // CSV mode parameters structure
    struct CSVParams {
        int32_t velocity;
        int32_t acceleration;
        int32_t deceleration;
    };

    // CST mode parameters structure
    struct CSTParams {
        int32_t max_torque;
        int32_t torque_slope;
    };

    // Parameter setting functions for each mode
    bool setPPModeParams(uint16_t slave, const PPParams& params);
    bool setPVModeParams(uint16_t slave, const PVParams& params);
    bool setPTModeParams(uint16_t slave, const PTParams& params);
    bool setCSPModeParams(uint16_t slave, const CSPParams& params);
    bool setCSVModeParams(uint16_t slave, const CSVParams& params);
    bool setCSTModeParams(uint16_t slave, const CSTParams& params);

    // Set operation mode
    bool setOperationMode(uint16_t slave, uint8_t mode);

    // Error handling
    std::string getLastError() const { return lastError; }

    // CST mode parameter setting functions
    bool setCSTParams(const CSTParams& params);
    bool setCSTParams(uint16_t slave, const CSTParams& params);

private:
    SDOManager() {} // Private constructor
    std::string lastError;

    static char IOmap[4096];

    // Helper function
    bool writeSDO(uint16_t slave, uint16_t index, uint8_t subindex, 
                  int size, void* data, const char* description);
};