# Qt GUI CMake Configuration

cmake_minimum_required(VERSION 3.16)

# 项目配置
project(calc_qt_gui VERSION 1.0.0 LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt 配置
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 自动化
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# AUTOMOC 额外的包含路径
set(CMAKE_AUTOMOC_ADDITIONAL_INCLUDE_PREFIXES
    ${CMAKE_SOURCE_DIR}/include
)

# 收集源文件
set(CORE_SOURCES
    core/calculator_controller.cpp
    core/calc_engine_adapter.cpp
)

set(WIDGET_SOURCES
    widgets/main_window.cpp
    widgets/display_widget.cpp
    widgets/keypad_widget.cpp
    widgets/mode_selector.cpp
    widgets/history_widget.cpp
    widgets/history_button.cpp
    widgets/history_modal.cpp
    widgets/function_panel.cpp
    widgets/converter_panel.cpp
)

# Widget headers for MOC processing
set(WIDGET_HEADERS
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/widgets/main_window.h
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/widgets/display_widget.h
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/widgets/keypad_widget.h
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/widgets/mode_selector.h
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/widgets/history_widget.h
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/widgets/history_button.h
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/widgets/history_modal.h
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/widgets/function_panel.h
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/widgets/converter_panel.h
)

# Mark headers for AUTOMOC
set_source_files_properties(${WIDGET_HEADERS} PROPERTIES AUTOMOC ON)

set(MODEL_SOURCES
    models/history_model.cpp
    models/mode_model.cpp
)

set(DIALOG_SOURCES
    dialogs/settings_dialog.cpp
    dialogs/about_dialog.cpp
)

# Dialog headers for MOC processing
set(DIALOG_HEADERS
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/dialogs/settings_dialog.h
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/dialogs/about_dialog.h
)

# Mark headers for AUTOMOC
set_source_files_properties(${DIALOG_HEADERS} PROPERTIES AUTOMOC ON)

set(RESOURCE_FILES
    resources/styles.qrc
)

# Core headers for MOC processing
set(CORE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/calc_engine_adapter.h
    ${CMAKE_SOURCE_DIR}/include/calc/ui/qt/core/calculator_controller.h
)

# Mark headers for AUTOMOC
set_source_files_properties(${CORE_HEADERS} PROPERTIES AUTOMOC ON)

# 创建 Qt GUI 可执行文件
add_executable(calc_qt_gui
    main.cpp
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${WIDGET_SOURCES}
    ${WIDGET_HEADERS}
    ${MODEL_SOURCES}
    ${DIALOG_SOURCES}
    ${DIALOG_HEADERS}
    ${RESOURCE_FILES}
)

# 添加头文件目录
target_include_directories(calc_qt_gui PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接库
target_link_libraries(calc_qt_gui PRIVATE
    Qt6::Core
    Qt6::Widgets
    calc_cli_lib  # CLI UI 库（共享历史管理等）
    calc_modes    # 计算模式
    calc_core     # 核心引擎
    calc_utils    # 工具类
)

# 设置输出目录
set_target_properties(calc_qt_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)

# macOS 特定设置
if(APPLE)
    set_target_properties(calc_qt_gui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.calc.qtgui"
        MACOSX_BUNDLE_BUNDLE_NAME "Calc"
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_VERSION ${PROJECT_VERSION}
    )
endif()

# Windows 特定设置
if(WIN32)
    set_target_properties(calc_qt_gui PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# 打印配置信息
message(STATUS "")
message(STATUS "========================================")
message(STATUS "  Qt GUI Configuration")
message(STATUS "========================================")
message(STATUS "  Project: ${PROJECT_NAME}")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Qt Version: ${Qt6_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "========================================")
message(STATUS "")
