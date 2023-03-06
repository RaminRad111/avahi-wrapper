cmake_minimum_required(VERSION 3.9)
project(avahi-wrapper)

set(CMAKE_CXX_STANDARD 17)

# Set the path to the header files
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find the required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(AVAHI REQUIRED avahi-client)

# Add Avahi client library and headers
include_directories(${AVAHI_INCLUDE_DIRS})
link_directories(${AVAHI_LIBRARY_DIRS})

# Create a library with your source files
add_library(avahi-wrapper SHARED
  src/mdns.cpp
  src/util.cpp
  include/avahi_wrapper/mdns.hpp
  include/avahi_wrapper/util.hpp
)

# Link the library with the required packages
target_link_libraries(${PROJECT_NAME} ${AVAHI_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include") 

# Create an executable for your example program
add_executable(publish_test example/publish_test.cpp)
add_executable(browse_test example/browse_test.cpp)

# Link the executable with your library and the required packages
target_link_libraries(publish_test avahi-wrapper ${AVAHI_LIBRARIES})
target_link_libraries(browse_test avahi-wrapper ${AVAHI_LIBRARIES})
