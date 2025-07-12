#pragma once
#include <cstdint>

#include "ethercat.h"

class DCManager {
private:
    DCManager() = default;
    ~DCManager() = default;
    DCManager(const DCManager&) = delete;
    DCManager& operator=(const DCManager&) = delete;

public:
    static DCManager& getInstance() {
        static DCManager instance;
        return instance;
    }

    bool configureDC();
    bool setupDCSync0(int slave, bool active, uint32_t cycleTime, int32_t shiftTime);
    void printDCStatus() const;
}; 