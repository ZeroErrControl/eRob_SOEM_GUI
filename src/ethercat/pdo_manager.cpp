#include "pdo_manager.h"

#include <cstdio>
#include <cstring>  // Header for memcpy

// Only define static IOmap
char PDOManager::IOmap[4096];

PDOManager::PDOManager() {
    // Initialize member variables
    memset(&rxpdo, 0, sizeof(RxPDO));
    memset(&txpdo, 0, sizeof(TxPDO));
}

bool PDOManager::configureRxPDO(int slave) {
    printf("Configuring RxPDO for slave %d...\n", slave);
    int retval = 0;
    uint16_t map_1c12;
    uint8_t zero_map = 0;
    uint32_t map_object;
    uint16_t clear_val = 0x0000;

    // Clear existing mapping
    retval += ec_SDOwrite(slave, 0x1600, 0x00, FALSE, sizeof(zero_map), &zero_map, EC_TIMEOUTSAFE);
    
    // Control word (0x6040:0, 16 bits)
    map_object = 0x60400010;
    retval += ec_SDOwrite(slave, 0x1600, 0x01, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);
    
    // Target position (0x607A:0, 32 bits)
    map_object = 0x607A0020;
    retval += ec_SDOwrite(slave, 0x1600, 0x02, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);
    
    // Target velocity (0x60FF:0, 32 bits)
    map_object = 0x60FF0020;
    retval += ec_SDOwrite(slave, 0x1600, 0x03, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);
    
    // Target torque (0x6071:0, 16 bits)
    map_object = 0x60710010;
    retval += ec_SDOwrite(slave, 0x1600, 0x04, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);

    // Operation mode (0x6060:0, 8 bits)
    map_object = 0x60600008;
    retval += ec_SDOwrite(slave, 0x1600, 0x05, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);
    
    // Padding bits (8 bits padding)
    map_object = 0x00000008;
    retval += ec_SDOwrite(slave, 0x1600, 0x06, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);
    
    // Set number of mapping objects
    uint8_t map_count = 6;  // Includes all objects
    retval += ec_SDOwrite(slave, 0x1600, 0x00, FALSE, sizeof(map_count), &map_count, EC_TIMEOUTSAFE);
    
    // Configure RXPDO assignment
    clear_val = 0x0000;
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, FALSE, sizeof(clear_val), &clear_val, EC_TIMEOUTSAFE);
    map_1c12 = 0x1600;
    retval += ec_SDOwrite(slave, 0x1c12, 0x01, FALSE, sizeof(map_1c12), &map_1c12, EC_TIMEOUTSAFE);
    map_1c12 = 0x0001;
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, FALSE, sizeof(map_1c12), &map_1c12, EC_TIMEOUTSAFE);

    if (retval < 0) {
        printf("RxPDO mapping failed for slave %d\n", slave);
        return false;
    }
    printf("RxPDO mapping completed for slave %d\n", slave);
    return true;
}

bool PDOManager::configureTxPDO(int slave) {
    printf("Configuring TxPDO for slave %d...\n", slave);
    int retval = 0;
    uint16_t map_1c13;
    uint32_t map_object;
    uint16_t clear_val = 0x0000;

    // Clear existing mapping
    retval += ec_SDOwrite(slave, 0x1A00, 0x00, FALSE, sizeof(clear_val), &clear_val, EC_TIMEOUTSAFE);

    // Status word (0x6041:0, 16 bits)
    map_object = 0x60410010;
    retval += ec_SDOwrite(slave, 0x1A00, 0x01, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);

    // Actual position (0x6064:0, 32 bits)
    map_object = 0x60640020;
    retval += ec_SDOwrite(slave, 0x1A00, 0x02, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);

    // Actual velocity (0x606C:0, 32 bits)
    map_object = 0x606C0020;
    retval += ec_SDOwrite(slave, 0x1A00, 0x03, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);

    // Actual torque (0x6077:0, 16 bits)
    map_object = 0x60770010;
    retval += ec_SDOwrite(slave, 0x1A00, 0x04, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);

    // Mode display (0x6061:0, 8 bits)
    map_object = 0x60610008;
    retval += ec_SDOwrite(slave, 0x1A00, 0x05, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);

    // Padding (8 bits padding)
    map_object = 0x00000008;
    retval += ec_SDOwrite(slave, 0x1A00, 0x06, FALSE, sizeof(map_object), &map_object, EC_TIMEOUTSAFE);

    // Set number of mapping objects
    uint8_t map_count = 6;  // Updated to 6 objects (including padding)
    retval += ec_SDOwrite(slave, 0x1A00, 0x00, FALSE, sizeof(map_count), &map_count, EC_TIMEOUTSAFE);

    // Configure TXPDO assignment
    clear_val = 0x0000;
    retval += ec_SDOwrite(slave, 0x1C13, 0x00, FALSE, sizeof(clear_val), &clear_val, EC_TIMEOUTSAFE);
    map_1c13 = 0x1A00;
    retval += ec_SDOwrite(slave, 0x1C13, 0x01, FALSE, sizeof(map_1c13), &map_1c13, EC_TIMEOUTSAFE);
    map_1c13 = 0x0001;
    retval += ec_SDOwrite(slave, 0x1C13, 0x00, FALSE, sizeof(map_1c13), &map_1c13, EC_TIMEOUTSAFE);

    if (retval < 0) {
        printf("TxPDO mapping failed for slave %d\n", slave);
        return false;
    }
    printf("TxPDO mapping completed for slave %d\n", slave);
    return true;
}

bool PDOManager::configureMapping() {
    printf("Configuring PDO mapping...\n");
    
    // Configure PDO for each slave
    for(int i = 1; i <= ec_slavecount; i++) {
        if (!PDOManager::configureRxPDO(i)) {
            return false;
        }
        if (!PDOManager::configureTxPDO(i)) {
            return false;
        }
    }

    // Configure IOmap
    ec_config_map(&IOmap);
    // Give slaves some time to process PDO configuration
    osal_usleep(100000);  // 100ms
    printf("PDO mapping completed successfully\n");
    return true;
}

bool PDOManager::initializePDO() {
    // Initialize RxPDO data
    rxpdo.controlword = 0x0000;        // Initial state: Off
    rxpdo.target_velocity = 0;         // Initial velocity: 0
    rxpdo.mode_of_operation = 9;       // CSV mode
    rxpdo.padding = 0;

    // Write initial data to slave
    for (int slave = 1; slave <= ec_slavecount; slave++) {
        memcpy(ec_slave[slave].outputs, &rxpdo, sizeof(RxPDO));
    }

    return true;
}

bool PDOManager::readProcessData(TxPDO& out_txpdo) {
    // Read data from first slave
    if (ec_slave[1].state == EC_STATE_OPERATIONAL) {
        memcpy(&txpdo, ec_slave[1].inputs, sizeof(TxPDO));
        out_txpdo = txpdo;
        return true;
    }
    return false;
}

bool PDOManager::writeProcessData(const RxPDO& in_rxpdo) {
    if (ec_slave[1].state == EC_STATE_OPERATIONAL) {
        rxpdo = in_rxpdo;
        memcpy(ec_slave[1].outputs, &rxpdo, sizeof(RxPDO));
        return true;
    }
    return false;
}

bool PDOManager::configurePDOs(int slave) {
    // PDO mapping configuration
    uint16_t map_1c12[2] = {0x01, 0x1600};
    uint16_t map_1c13[2] = {0x01, 0x1A00};
    
    if (ec_SDOwrite(slave, 0x1c12, 0, FALSE, sizeof(map_1c12), &map_1c12, EC_TIMEOUTRXM) <= 0) {
        return false;
    }
    
    if (ec_SDOwrite(slave, 0x1c13, 0, FALSE, sizeof(map_1c13), &map_1c13, EC_TIMEOUTRXM) <= 0) {
        return false;
    }
    
    return true;
}