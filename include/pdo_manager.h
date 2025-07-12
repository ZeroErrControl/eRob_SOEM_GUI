#pragma once

#include "ethercat.h"

class PDOManager {
public:
    // PDO 数据结构定义
    typedef struct {
        uint16_t controlword;       // 0x6040:0, 16 bits
        int32_t target_position;    // 0x607A:0, 32 bits
        int32_t target_velocity;    // 0x60FF:0, 32 bits
        int16_t target_torque;      // 0x6071:0, 16 bits
        uint8_t mode_of_operation;  // 0x6060:0, 8 bits
        uint8_t padding;           // 8 bits padding
    } __attribute__((__packed__)) RxPDO;

    typedef struct {
        uint16_t statusword;       // 0x6041:0, 16 bits
        int32_t actual_position;   // 0x6064:0, 32 bits
        int32_t actual_velocity;   // 0x606C:0, 32 bits
        int16_t actual_torque;     // 0x6077:0, 16 bits
        uint8_t mode_of_operation_display;   // 0x6061:0, 8 bits
        uint8_t padding;           // 8 bits padding
    } __attribute__((__packed__)) TxPDO;

    PDOManager();  // 构造函数

    // 静态配置方法
    static bool configureMapping();
    static bool configureRxPDO(int slave);
    static bool configureTxPDO(int slave);
    
    bool initializePDO();
    bool readProcessData(TxPDO& txpdo);
    bool writeProcessData(const RxPDO& rxpdo);

    static bool configurePDOs(int slave);

private:
    RxPDO rxpdo;
    TxPDO txpdo;
    static char IOmap[4096];
};

struct SharedData {
    struct DataPoint {
        int32_t position;
        int32_t velocity;
        int16_t torque;
        int8_t mode;  // 添加这个字段
        
        DataPoint() : position(0), velocity(0), torque(0), mode(0) {}
    };
    // ... 其他成员 ...
};