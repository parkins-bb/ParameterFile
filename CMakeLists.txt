cmake_minimum_required(VERSION 3.18)
project(my_parameter)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 处理CMP0144警告
cmake_policy(SET CMP0074 NEW)

# 设置Boost根目录
set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/include")

# 指定Boost和yaml-cpp的头文件目录, 指定Parameter头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 添加静态库
add_library(yaml-cpp STATIC IMPORTED)
set_property(TARGET yaml-cpp PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libyaml-cpp.a")

# 手动设置Boost组件的路径（这是由于没有使用系统安装的Boost）
set(Boost_INCLUDE_DIR "${BOOST_ROOT}")
set(Boost_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
find_package(Boost 1.54.0 REQUIRED COMPONENTS filesystem system)  # 设置Boost版本

# 查找并添加Boost库
if(Boost_FOUND)
	# 包含Boost库头文件目录
	include_directories(${Boost_INCLUDE_DIRS})   # 注意，此处为{Boost_INCLUDE_DIRS}，这是我的find_package()找到的所有头文件
	link_directories(${Boost_LIBRARY_DIR})
endif()

# 添加Parameter库
add_library(parameter_lib src/Parameter.cpp)

# 链接yaml-cpp静态库和Boost库到Parameter库
target_link_libraries(parameter_lib yaml-cpp ${Boost_LIBRARIES})

# 设置二进制文件的目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 添加example可执行文件
add_executable(example_bin example/example_2.cpp)

# 链接Parameter库到example_bin可执行文件
target_link_libraries(example_bin parameter_lib)

