cmake_minimum_required(VERSION 3.20)
project(server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)

# Boost
find_package(Boost REQUIRED system thread)

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# ImGui
add_subdirectory(external/imgui)
target_include_directories(imgui PUBLIC ${glfw_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h src/*.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${BOOST_INCLUDE_DIR}
  ${GLFW3_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
  Boost::system
  Boost::thread
  glfw
  imgui
  OpenGL::GL
)