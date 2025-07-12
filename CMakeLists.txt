# 指定最低 CMake 版本要求
cmake_minimum_required(VERSION 3.9)

# 项目定义
project(MotorControl
    VERSION 1.0.0                          # 项目版本
    DESCRIPTION "Motor Control Upper Computer"  # 项目描述
    LANGUAGES CXX C                        # 使用的语言：C++ 和 C
)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)                 # 使用 C++14
set(CMAKE_CXX_STANDARD_REQUIRED ON)        # 强制要求使用指定的 C++ 标准

# 设置输出目录
# 注意：使用 CMAKE_SOURCE_DIR 而不是 CMAKE_BINARY_DIR，这样输出会在项目根目录下
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)  # 可执行文件输出到 ./bin
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)  # 动态库输出到 ./lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)  # 静态库输出到 ./lib

# 启用 Qt 自动处理系统
# 这些必须在任何 target 定义之前设置
set(CMAKE_AUTOMOC ON)                      # 启用 Qt 的 MOC 系统
set(CMAKE_AUTORCC ON)                      # 启用 Qt 资源文件处理
set(CMAKE_AUTOUIC ON)                      # 启用 Qt UI 文件处理

# 查找 Qt 包
# COMPONENTS 指定需要的 Qt 模块
# REQUIRED 表示这些是必需的，如果找不到会报错
find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)

# 添加 SOEM 库
# 注意：这必须在使用 SOEM 的其他目标之前添加
add_subdirectory(third_party/SOEM)

# 添加 QCustomPlot 库
# 注意：这是一个静态库，需要与 Qt 的部件一起使用
set(QCUSTOMPLOT_DIR ${CMAKE_SOURCE_DIR}/third_party/qcustomplot)
add_library(qcustomplot STATIC
    ${QCUSTOMPLOT_DIR}/qcustomplot.cpp
    ${QCUSTOMPLOT_DIR}/qcustomplot.h
)

# 设置 QCustomPlot 的链接选项
target_link_libraries(qcustomplot
    PUBLIC                  # PUBLIC 使依赖传递给链接 qcustomplot 的目标
    Qt5::Widgets
    Qt5::PrintSupport
)

# 设置 QCustomPlot 的包含目录
target_include_directories(qcustomplot
    PUBLIC
    ${QCUSTOMPLOT_DIR}
)

# 设置全局包含目录
# 注意：最好使用 target_include_directories 而不是全局 include_directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include            # 项目头文件目录
    ${CMAKE_SOURCE_DIR}/third_party/SOEM   # SOEM 头文件
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/soem
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/osal
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/osal/linux
    ${CMAKE_SOURCE_DIR}/third_party/SOEM/oshw/linux
)

# 添加子目录
# 注意：子目录的添加顺序很重要，被依赖的目录应该先添加
add_subdirectory(src)                      # 核心库和主程序

# 安装规则
# 注意：使用 CMAKE_SOURCE_DIR 确保安装到项目目录下
install(TARGETS soem qcustomplot
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib    # 动态库
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib    # 静态库
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin    # 可执行文件
)

# 安装头文件
# 注意：只安装 .h 文件
install(DIRECTORY include/
    DESTINATION ${CMAKE_SOURCE_DIR}/include
    FILES_MATCHING PATTERN "*.h"
)

# 注意事项：
# 1. 确保所有路径使用变量而不是硬编码
# 2. 注意依赖关系的顺序
# 3. 使用 target_* 命令而不是全局命令
# 4. 注意权限和安装路径
# 5. 保持良好的结构和命名规范

