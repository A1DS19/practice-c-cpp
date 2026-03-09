cmake_minimum_required(VERSION 3.25)

# Dynamically name the project after the folder
get_filename_component(PROJECT_NAME ${CMAKE_SOURCE_DIR} NAME)
project(
  ${PROJECT_NAME}
  VERSION 1.0.0
  DESCRIPTION "Modern C++ Project"
  LANGUAGES CXX)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Enable compile_commands.json for LSP support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Modern C++ standard (change to 20 if C++23 is not available)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wunused
    -Wuninitialized
    -Wshadow
    -Wformat=2)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Werror -fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
  else()
    add_compile_options(-O3 -DNDEBUG)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wunused
    -Wuninitialized
    -Wshadow
    -Wformat=2)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Werror -fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
  else()
    add_compile_options(-O3 -DNDEBUG)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/W4 /permissive-)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(/Od /RTC1 /WX)
  else()
    add_compile_options(/O2 /DNDEBUG)
  endif()
endif()

# Dependencies OpenGL: the graphics API ImGui renders through glfw3: window
# creation and input handling (keyboard, mouse, etc.)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# ImGui - compiled as a static library with relaxed warnings ImGui is a
# "immediate mode" GUI library: you describe your UI every frame rather than
# managing widget state. It lives in vendor/ so it is never cleaned. We list
# each .cpp explicitly because imgui_demo.cpp is optional (demo window) and we
# don't want to blindly glob everything.
file(
  GLOB
  IMGUI_SOURCES
  ${CMAKE_SOURCE_DIR}/vendor/imgui/src/imgui.cpp # core
  ${CMAKE_SOURCE_DIR}/vendor/imgui/src/imgui_draw.cpp # draw lists / rendering
                                                      # primitives
  ${CMAKE_SOURCE_DIR}/vendor/imgui/src/imgui_tables.cpp # table widget
  ${CMAKE_SOURCE_DIR}/vendor/imgui/src/imgui_widgets.cpp # buttons, sliders,
                                                         # inputs, etc.
  ${CMAKE_SOURCE_DIR}/vendor/imgui/src/imgui_impl_glfw.cpp # backend: maps GLFW
                                                           # events -> ImGui
  ${CMAKE_SOURCE_DIR}/vendor/imgui/src/imgui_impl_opengl3.cpp) # backend:
                                                               # uploads draw
                                                               # data to OpenGL
add_library(imgui STATIC ${IMGUI_SOURCES})
# PUBLIC so that any target linking imgui automatically gets the include path
target_include_directories(imgui
                           PUBLIC ${CMAKE_SOURCE_DIR}/vendor/imgui/include)
# glfw and OpenGL::GL are propagated to targets that link imgui (also PUBLIC)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
# -w silences all compiler warnings for vendor code we don't own or want to fix
set_target_properties(imgui PROPERTIES COMPILE_FLAGS "-w")

# Include directories Makes #include "main.hpp" work without a path prefix
include_directories(${CMAKE_SOURCE_DIR}/include)

# Gather all source files CONFIGURE_DEPENDS re-runs CMake automatically when
# files are added/removed
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp
     ${CMAKE_SOURCE_DIR}/src/*.cxx)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/include/*.hpp
     ${CMAKE_SOURCE_DIR}/include/*.h)

# Define the executable
add_executable(main ${SOURCES} ${HEADERS})

# Target-specific properties
set_target_properties(
  main
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
             CXX_STANDARD 23
             CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS OFF)

# Platform-specific configurations
if(WIN32)
  target_compile_definitions(main PRIVATE WIN32_LEAN_AND_MEAN)
elseif(APPLE)
  set_target_properties(main PROPERTIES MACOSX_RPATH TRUE)
elseif(UNIX)
  target_link_libraries(main PRIVATE dl)
endif()

target_link_libraries(main PRIVATE imgui)

# Copy compile_commands.json to project root for LSP
if(EXISTS ${CMAKE_BINARY_DIR}/compile_commands.json)
  add_custom_command(
    TARGET main
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_BINARY_DIR}/compile_commands.json
      ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Copying compile_commands.json to project root")
endif()

# Optional: Enable testing
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
  enable_testing()
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
         ${CMAKE_SOURCE_DIR}/tests/*.cpp)
    add_executable(test_main ${TEST_SOURCES})
    target_include_directories(test_main PRIVATE ${CMAKE_SOURCE_DIR}/include)
    set_target_properties(test_main PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                               ${CMAKE_SOURCE_DIR}/bin)
    add_test(NAME unit_tests COMMAND test_main)
  endif()
endif()

# Development target for setup
add_custom_target(
  dev-setup
  COMMAND ${CMAKE_COMMAND} -E echo "Setting up development environment..."
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
  COMMENT "Development setup complete")

# Print configuration summary
message(STATUS "")
message(STATUS "Configuration Summary:")
message(STATUS "  Project Name:     ${PROJECT_NAME}")
message(STATUS "  Build Type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard:     ${CMAKE_CXX_STANDARD}")
message(
  STATUS
    "  Compiler:         ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
)
message(STATUS "  Source Directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "  Binary Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "")
