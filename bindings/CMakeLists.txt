cmake_minimum_required(VERSION 3.15)

# Python 绑定模块
pybind11_add_module(fastquant_cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings_main.cpp
)

target_link_libraries(fastquant_cpp PRIVATE fastquant_core)

# 设置输出目录，方便 Python 导入
set_target_properties(fastquant_cpp PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/app
)

# 安装到 Python site-packages
install(TARGETS fastquant_cpp LIBRARY DESTINATION .)
