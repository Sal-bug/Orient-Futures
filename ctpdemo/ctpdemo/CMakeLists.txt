# Minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.0)

# Name of the project
project(CtpDemo)

file(GLOB CtpDemo_SRC CONFIGURE_DEPENDS "*.cpp")
file(GLOB CtpDemo_LIB CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/api/lib*.so")

# Add all the source files needed to build the executable
add_executable(CtpDemo ${CtpDemo_SRC})

# Add all the additional directories
include_directories(CtpDemo PRIVATE ./api)

# Include external link libraries
# link_directories(${PROJECT_SOURCE_DIR}/api)
target_link_libraries(CtpDemo ${CtpDemo_LIB})

# find_library(CURL_LIBRARY
#     NAMES thosttraderapi_se.so
#     HINTS "${PROJECT_SOURCE_DIR}"
# )

# message(STATUS "foo include dir: ${DemoLib}")

