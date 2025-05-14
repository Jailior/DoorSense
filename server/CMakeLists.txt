cmake_minimum_required(VERSION 3.20)
project(server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# include(FetchContent)

# # Boost
# FetchContent_Declare(
#   Boost
#   GIT_REPOSITORY https://github.com/boostorg/boost.git
#   GIT_PROGRESS TRUE
#   GIT_TAG boost-1.82.0
# )
# FetchContent_MakeAvailable(Boost)

find_package(Boost REQUIRED system thread)

# Source files
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h src/*.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${BOOST_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
  Boost::system
  Boost::thread
)