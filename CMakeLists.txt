cmake_minimum_required(VERSION 3.4.2)
project(micros_pluginManager VERSION 0.1.0)

include(../micros.cmake)
message(MICROS_VERSION: ${MICROS_VERSION})

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -pthread -lboost_filesystem)
endif()


include_directories( include /usr/include/ "${PROJECT_SOURCE_DIR}/include")
#find(tinyxml2 REQUIRED)

set(LINK_DIR lib ) # can not be obsolute path
set(CONSOLE_DIR /lib/x86_64-linux-gnu/)
set(POCO_DIR /usr/lib/x86_64-linux-gnu/)
set(MICROS_PLUGIN_LIB_DIR ${CMAKE_INSTALL_RPATH}/plugins)

link_directories(${LINK_DIR} ${CONSOLE_DIR} ${POCO_DIR} ${MICROS_PLUGIN_LIB_DIR})


set(${PROJECT_NAME}_SRCS
  test/labelClassTest.cpp
  src/label.cpp
)

set(${PROJECT_NAME}_HDRS
  ${PROJECT_SOURCE_DIR}/include/label.h
  ${PROJECT_SOURCE_DIR}/include/constParam.h
)


add_executable(labelClassTest ${${PROJECT_NAME}_SRCS})

add_executable(tinyxml2Test test/tinyxml2test.cpp)
target_link_libraries(tinyxml2Test optimized libtinyxml2.so)

add_executable(sensorClassTest test/sensorClassTest.cpp src/sensor.cpp)

add_executable(labelGeneratorPluginLoadTest test/labelGeneratorPluginLoadTest.cpp src/label.cpp src/pluginManager.cpp src/sensor.cpp)

target_link_libraries(labelGeneratorPluginLoadTest 
  libclass_loader.so #must include, plugin machinism need it
  libconsole_bridge.so #must include, libclass_loader.so need it
  libPocoFoundation.so #must include, libclass_loader.so need it
  libtinyxml2.so #must include, xml parse need it
  libboost_filesystem.so #must include, boost::filesystem::path and system_complete need it
  libpthread.so
)

add_executable(sensorPluginXMLParseTest test/sensorPluginXMLParseTest.cpp src/sensor.cpp src/label.cpp src/pluginManager.cpp)

target_link_libraries(sensorPluginXMLParseTest 
  libclass_loader.so #must include, plugin machinism need it
  libconsole_bridge.so #must include, libclass_loader.so need it
  libPocoFoundation.so #must include, libclass_loader.so need it
  libtinyxml2.so #must include, xml parse need it
  libboost_filesystem.so #must include, boost::filesystem::path and system_complete need it
  libpthread.so
)

add_executable(sensorPluginLoadTest test/sensorPluginLoadTest.cpp src/label.cpp src/pluginManager.cpp src/sensor.cpp )

target_link_libraries(sensorPluginLoadTest 
  libclass_loader.so #must include, plugin machinism need it
  libconsole_bridge.so #must include, libclass_loader.so need it
  libPocoFoundation.so #must include, libclass_loader.so need it
  libtinyxml2.so #must include, xml parse need it
  libboost_filesystem.so #must include, boost::filesystem::path and system_complete need it
  libpthread.so
)

add_executable(integrateTest test/integrateTest.cpp src/label.cpp src/pluginManager.cpp src/sensor.cpp )

target_link_libraries(integrateTest 
  libclass_loader.so #must include, plugin machinism need it
  libconsole_bridge.so #must include, libclass_loader.so need it
  libPocoFoundation.so #must include, libclass_loader.so need it
  libtinyxml2.so #must include, xml parse need it
  libboost_filesystem.so #must include, boost::filesystem::path and system_complete need it
  libpthread.so
)

add_executable(test_v4l2_plugin_manager test/v4l2Test.cpp src/label.cpp src/pluginManager.cpp src/sensor.cpp)

target_link_libraries(test_v4l2_plugin_manager 
  libclass_loader.so #must include, plugin machinism need it
  libconsole_bridge.so #must include, libclass_loader.so need it
  libPocoFoundation.so #must include, libclass_loader.so need it
  libtinyxml2.so #must include, xml parse need it
  libboost_filesystem.so #must include, boost::filesystem::path and system_complete need it
  libpthread.so
) 

add_executable(apiTest test/apiTest.cpp src/label.cpp src/pluginManager.cpp src/sensor.cpp)

target_link_libraries(apiTest 
  libclass_loader.so #must include, plugin machinism need it
  libconsole_bridge.so #must include, libclass_loader.so need it
  libPocoFoundation.so #must include, libclass_loader.so need it
  libtinyxml2.so #must include, xml parse need it
  libboost_filesystem.so #must include, boost::filesystem::path and system_complete need it
  libpthread.so
) 

add_library(pluginManager.0.1.0 src/label.cpp src/pluginManager.cpp src/sensor.cpp)
target_link_libraries(pluginManager.0.1.0
  libclass_loader.so #must include, plugin machinism need it
  libconsole_bridge.so #must include, libclass_loader.so need it
  libPocoFoundation.so #must include, libclass_loader.so need it
  libtinyxml2.so #must include, xml parse need it
  libboost_filesystem.so #must include, boost::filesystem::path and system_complete need it
  libpthread.so
) 

install(TARGETS test_v4l2_plugin_manager RUNTIME DESTINATION bin/plugin_test)

install(TARGETS pluginManager.0.1.0 LIBRARY DESTINATION lib/plugins)

