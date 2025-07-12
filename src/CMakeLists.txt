# 查找必需的包
# 注意：这些包的查找应该在顶层 CMakeLists.txt 中完成，这里是为了确保依赖
find_package(Qt5 COMPONENTS Core Widgets PrintSupport Concurrent Gui Network REQUIRED)
find_package(Threads REQUIRED)

# 启用 Qt 自动处理系统
# 注意：这些设置会被继承，但为了安全起见最好在这里也设置
set(CMAKE_AUTOMOC ON)                      # 启用 Qt 的 MOC 系统
set(CMAKE_AUTORCC ON)                      # 启用 Qt 资源文件处理
set(CMAKE_AUTOUIC ON)                      # 启用 Qt UI 文件处理

# 添加库的源文件
# 注意：随着项目发展，需要在这里添加新的源文件
set(LIB_SOURCES
    ethercat/ethercat_thread.cpp
    ethercat/ethercat_manager.cpp
    ethercat/pdo_manager.cpp
    ethercat/dc_manager.cpp
    ethercat/sdo_manager.cpp
    qt_ui/components/component_manager.cpp   # 已移动到新位置
)

# 添加 GUI 相关源文件
set(GUI_SOURCES
    qt_ui/monitor_window.cpp
    qt_ui/monitor_window_ui.cpp
    qt_ui/monitor_window_events.cpp
    qt_ui/monitor_window_data.cpp
    qt_ui/plot/plot_manager.cpp
    qt_ui/control/motor_controller.cpp
    qt_ui/control/mode_manager.cpp
    qt_ui/network/network_manager.cpp
    qt_ui/logging/log_manager.cpp
    main.cpp
)

# 添加需要 MOC 处理的头文件
# 注意：所有包含 Q_OBJECT 宏的头文件都需要在这里列出
set(MOC_HEADERS
    ${CMAKE_SOURCE_DIR}/include/monitor_window.h
    ${CMAKE_SOURCE_DIR}/include/qt_ui/monitor_window_events.h
    ${CMAKE_SOURCE_DIR}/include/qt_ui/monitor_window_data.h
    ${CMAKE_SOURCE_DIR}/include/ethercat_thread.h
    ${CMAKE_SOURCE_DIR}/include/qt_ui/plot/plot_manager.h
    ${CMAKE_SOURCE_DIR}/include/qt_ui/control/motor_controller.h
    ${CMAKE_SOURCE_DIR}/include/qt_ui/control/mode_manager.h
    ${CMAKE_SOURCE_DIR}/include/qt_ui/network/network_manager.h
    ${CMAKE_SOURCE_DIR}/include/qt_ui/logging/log_manager.h
)

# 确保 ethercat_thread.cpp 只在一个目标中编译
set(COMMON_SOURCES
    ethercat/ethercat_manager.cpp
    ethercat/dc_manager.cpp
    ethercat/pdo_manager.cpp
    ethercat/sdo_manager.cpp
)

# 添加 QCustomPlot 源文件
set(QCUSTOMPLOT_SOURCES
    ${CMAKE_SOURCE_DIR}/third_party/qcustomplot/qcustomplot.cpp
)

# 添加 MOC 源文件到 ethercat_core
add_library(ethercat_core SHARED
    ${LIB_SOURCES}
    ${MOC_HEADERS}  # 添加 MOC 头文件
)

# 设置 ethercat_core 的属性
set_target_properties(ethercat_core PROPERTIES
    AUTOMOC ON    # 确保启用 MOC
    AUTORCC ON
    AUTOUIC ON
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
)

# 设置包含目录
target_include_directories(ethercat_core
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/qt_ui
    ${CMAKE_SOURCE_DIR}/include/qt_ui/components
    ${CMAKE_SOURCE_DIR}/include/qt_ui/plot
    ${CMAKE_SOURCE_DIR}/include/qt_ui/control
    ${CMAKE_SOURCE_DIR}/include/qt_ui/network
    ${CMAKE_SOURCE_DIR}/include/qt_ui/logging
    ${CMAKE_SOURCE_DIR}/third_party/SOEM
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/soem
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/osal
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/osal/linux
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/oshw/linux
    ${CMAKE_SOURCE_DIR}/third_party/qcustomplot
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

# 设置链接选项
target_link_libraries(ethercat_core
    PUBLIC
    Qt5::Core
    Qt5::Widgets
    Qt5::Concurrent
    Qt5::PrintSupport
    soem
    pthread
    rt
)

# 添加所有源文件 - 合并GUI和库文件，避免重复
set(SOURCE_FILES
    main.cpp
    qt_ui/monitor_window.cpp
    qt_ui/monitor_window_ui.cpp
    qt_ui/monitor_window_events.cpp
    qt_ui/monitor_window_data.cpp
    qt_ui/components/component_manager.cpp
    qt_ui/plot/plot_manager.cpp
    qt_ui/control/motor_controller.cpp
    qt_ui/control/mode_manager.cpp
    qt_ui/network/network_manager.cpp
    qt_ui/logging/log_manager.cpp
    ethercat/ethercat_manager.cpp
    ethercat/dc_manager.cpp
    ethercat/pdo_manager.cpp
    ethercat/sdo_manager.cpp
    algorithms/csp_motion_planning.cpp  # 添加新的源文件
)

# 添加所有头文件目录 - 统一使用CMAKE_SOURCE_DIR
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/qt_ui
    ${CMAKE_SOURCE_DIR}/include/qt_ui/components
    ${CMAKE_SOURCE_DIR}/include/qt_ui/plot
    ${CMAKE_SOURCE_DIR}/include/qt_ui/control
    ${CMAKE_SOURCE_DIR}/include/qt_ui/network
    ${CMAKE_SOURCE_DIR}/include/qt_ui/logging
    ${CMAKE_SOURCE_DIR}/include/algorithms
    ${CMAKE_SOURCE_DIR}/third_party/SOEM
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/soem
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/osal
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/osal/linux
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/oshw/linux
    ${CMAKE_SOURCE_DIR}/third_party/qcustomplot
)

# 创建可执行文件
add_executable(ethercat_monitor ${SOURCE_FILES})

# 链接依赖库
target_link_libraries(ethercat_monitor
    PRIVATE
    ethercat_core
    Qt5::Widgets
    Qt5::PrintSupport
    Qt5::Network
    Qt5::Concurrent
    qcustomplot
)

# 设置 Qt 相关属性
set_target_properties(ethercat_monitor PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    POSITION_INDEPENDENT_CODE ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# 重要注意事项：
# 1. 源文件分组：
#    - LIB_SOURCES: EtherCAT核心库源文件
#    - GUI_SOURCES: GUI相关源文件
#    - MOC_HEADERS: 需要Qt MOC处理的头文件
#
# 2. 目标分组：
#    - ethercat_core: 核心功能库
#    - ethercat_monitor: GUI监控程序
#    - ethercat_test: 测试程序
#    - ethercat_backup: 备份程序
#
# 3. 输出目录：
#    - 库文件输出到 lib/
#    - 可执行文件输出到 bin/
#
# 4. 依赖关系：
#    - ethercat_monitor 依赖于 ethercat_core
#    - ethercat_test 依赖于 ethercat_core
#    - ethercat_backup 依赖于 ethercat_core
#    - 两者都依赖于 Qt 和 SOEM

