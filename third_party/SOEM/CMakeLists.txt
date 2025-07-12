cmake_minimum_required(VERSION 3.9)
project(SOEM VERSION 1.4.0 LANGUAGES C)

# Set build options
option(BUILD_SHARED_LIBS "Build shared library" ON)

# Source files for Linux platform
set(SOEM_SOURCES
    soem/ethercatbase.c
    soem/ethercatcoe.c
    soem/ethercatconfig.c
    soem/ethercatdc.c
    soem/ethercatfoe.c
    soem/ethercatmain.c
    soem/ethercatprint.c
    soem/ethercatsoe.c
    soem/ethercateoe.c
    osal/linux/osal.c
    oshw/linux/nicdrv.c
    oshw/linux/oshw.c
)

# Create SOEM library
add_library(soem ${SOEM_SOURCES})

# Include directories
target_include_directories(soem
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/soem
        ${CMAKE_CURRENT_SOURCE_DIR}/osal
        ${CMAKE_CURRENT_SOURCE_DIR}/osal/linux
        ${CMAKE_CURRENT_SOURCE_DIR}/oshw/linux
)

# Find and link required system libraries
find_package(Threads REQUIRED)
target_link_libraries(soem PRIVATE Threads::Threads rt)

# Set compile definitions for Linux
target_compile_definitions(soem PRIVATE OS_LINUX)

# Installation rules
install(TARGETS soem
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES 
    soem/ethercat.h
    soem/ethercatbase.h
    soem/ethercatcoe.h
    soem/ethercatconfig.h
    soem/ethercatconfiglist.h
    soem/ethercatdc.h
    soem/ethercateoe.h
    soem/ethercatfoe.h
    soem/ethercatmain.h
    soem/ethercatprint.h
    soem/ethercatsoe.h
    soem/ethercattype.h
    DESTINATION include/soem
)