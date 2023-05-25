get_filename_component(SELF_DIR ${CMAKE_CURRENT_LIST_DIR} PATH)

include(CMakeFindDependencyMacro)

find_dependency(Qt6 REQUIRED COMPONENTS Core Network Gui)

include(${SELF_DIR}/httpclient/httpclient.cmake)

