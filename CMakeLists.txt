cmake_minimum_required (VERSION 3.0)
project (moedacoin)

# find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# instruct CMake to run moc and uic automatically when needed.
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
include(FindOpenSSL)

# find the QtWidgets library
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql)

# src, headers, ui and res
set(SRC_PATH "src")
set(INCLUDE_PATH "include")
set(UI_PATH "ui")
set(RES_PATH "res")

file(GLOB SRC		"${SRC_PATH}/*.cpp")
file(GLOB HEADER	"${INCLUDE_PATH}/*.hpp")
file(GLOB UI		"${UI_PATH}/*.ui")
file(GLOB RES		"${RES_PATH}/*.qrc")
# message(STATUS "src: " ${SRC})
# message(STATUS "header: " ${HEADER})
# message(STATUS "res: " ${RES})

QT5_ADD_RESOURCES(RES_SOURCES ${RES})
QT5_WRAP_UI(UI_HEADERS ${UI})
QT5_WRAP_CPP(MOC_SRC ${HEADER})

# export information about compilation
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

# tell CMake to create the helloworld executable
add_executable(moedacoin ${SRC} ${RES_SOURCES} ${UI_HEADERS} ${MOC_SRC})

# include dirs
include_directories(${INCLUDE_PATH})

# use the Widgets module from Qt 5.
target_link_libraries(
	moedacoin Qt5::Core Qt5::Widgets Qt5::Sql ${OPENSSL_LIBRARIES})
