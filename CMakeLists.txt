cmake_minimum_required(VERSION 3.3)
PROJECT(simple-server)
set(TARGET_NAME simple-server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pg")

# 开启调试：-DDEBUG=ON
OPTION(DEBUG
        "Build the project using debugging code"
        OFF)
IF (DEBUG)
    MESSAGE("Adding Debug flag...")
    ADD_DEFINITIONS(-DDEBUG)
    SET(CMAKE_BUILD_TYPE Debug)
    MESSAGE("Build type is " ${CMAKE_BUILD_TYPE})
ENDIF (DEBUG)

# 将src目录下的所有文件赋值到SOURCE变量
aux_source_directory(src SOURCE)

# 指定连接库路径
link_directories(lib)

# 第三方代码
#add_subdirectory(third-party)

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.74.0 COMPONENTS log REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
#message(STATUS "${Boost_INCLUDE_DIRS}")

# include 路径
include_directories(./include)
include_directories(./src/utils)
add_executable(
        ${TARGET_NAME}
        ${SOURCE}
)
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
# 连接库
target_link_libraries(${TARGET_NAME}
        PUBLIC ${Boost_STACKTRACE_BASIC_LIBRARY}
        PUBLIC ${Boost_STACKTRACE_BACKTRACE_LIBRARY}
        PUBLIC ${Boost_STACKTRACE_ADDR2LINE_LIBRARY}
        PUBLIC ${Boost_STACKTRACE_NOOP_LIBRARY}
        PUBLIC ${Boost_LIBRARIES}
        PUBLIC Threads::Threads
        PUBLIC pthread dl)


#message(STATUS "123 ${Boost_LIBRARIES}")
