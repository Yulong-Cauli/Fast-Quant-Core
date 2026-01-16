cmake_minimum_required(VERSION 3.15)
project(FastQuantCore VERSION 0.1.0 LANGUAGES CXX)

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -O3)
    # 面试亮点：启用 Link Time Optimization
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# 输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找 Python 和 pybind11
find_package(Python3 COMPONENTS Interpreter Development)
if(Python3_FOUND)
    message(STATUS "Found Python3: ${Python3_VERSION}")
    
    # 尝试查找 pybind11
    find_package(pybind11 CONFIG QUIET)
    if(pybind11_FOUND)
        message(STATUS "Found pybind11: ${pybind11_VERSION}")
        set(BUILD_PYTHON_BINDINGS ON)
    else()
        message(STATUS "pybind11 not found, Python bindings will not be built")
        message(STATUS "Install with: pip install pybind11[global]")
        set(BUILD_PYTHON_BINDINGS OFF)
    endif()
else()
    message(STATUS "Python3 not found, Python bindings will not be built")
    set(BUILD_PYTHON_BINDINGS OFF)
endif()

# C++ 核心库（Header-Only，展示现代 C++ 设计）
add_library(fastquant_core INTERFACE)
target_include_directories(fastquant_core INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core/include>
    $<INSTALL_INTERFACE:include>
)

# 如果找到 pybind11，构建 Python 绑定
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(bindings)
endif()

# 测试（可选）
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 示例程序
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_executable(example_strategy_test 
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/example_strategy_test.cpp
    )
    target_link_libraries(example_strategy_test PRIVATE fastquant_core)
endif()

# 安装规则
install(DIRECTORY core/include/ DESTINATION include)
install(TARGETS fastquant_core EXPORT FastQuantCoreTargets)
install(EXPORT FastQuantCoreTargets
    FILE FastQuantCoreTargets.cmake
    NAMESPACE FastQuant::
    DESTINATION lib/cmake/FastQuantCore
)

# 打印配置摘要
message(STATUS "")
message(STATUS "FastQuantCore Configuration Summary:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python Bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  Build Examples: ${BUILD_EXAMPLES}")
message(STATUS "")
