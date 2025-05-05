cmake_minimum_required(VERSION 3.25)

project(httpclient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include(GNUInstallDirs)

set(SOURCES httpclient.cpp include/httpclient/httpclient.hpp)

find_package(Qt6 REQUIRED COMPONENTS Core Network Gui)

add_library(httpclient STATIC ${SOURCES})

target_include_directories(httpclient
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(httpclient PUBLIC Qt6::Core Qt6::Network Qt6::Gui)

# Generate the export file
install(TARGETS httpclient
  EXPORT httpclient
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# Export the project
install(EXPORT httpclient
  FILE httpclient.cmake
  NAMESPACE httpclient::
  DESTINATION lib/cmake/httpclient
)

# Install the header files to the installation directory
# Destinations are relative to CMAKE_INSTALL_PREFIX.
install(DIRECTORY 
    ${PROJECT_SOURCE_DIR}/include/ 
DESTINATION
    include
)

# Install config file
install(FILES httpclient-config.cmake DESTINATION lib/cmake/httpclient)