cmake_minimum_required(VERSION 3.18)
project(my_parameter)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include/yaml-cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 添加静态库
add_library(yaml-cpp STATIC IMPORTED)
set_property(TARGET yaml-cpp PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib/libyaml-cpp.a")

# 查找并添加Boost库
find_package(Boost REQUIRED COMPONENTS filesystem)
if(Boost_FOUND)
	# 包含Boost库头文件目录
	include_directories(${Boost_INCLUDE_DIRS})
endif()

# 添加Parameter库
add_library(parameter_lib
	src/Parameter.cpp
)

# 链接yaml-cpp静态库和Boost库到Parameter库
target_link_libraries(parameter_lib yaml-cpp ${Boost_LIBRARIES})

# 设置二进制文件的目录
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 添加example可执行文件
add_executable(example_bin example/example.cpp)

# 链接Parameter库到example_bin可执行文件
target_link_libraries(example_bin parameter_lib)


