cmake_minimum_required(VERSION 3.20)
project(hermes-cli VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable NuGet package management
find_package(PkgConfig QUIET)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# NuGet packages directory
set(NUGET_PACKAGES_DIR "${CMAKE_BINARY_DIR}/packages")

# Download and extract NuGet package
set(HERMES_PACKAGE_VERSION "0.0.0-2508.18001-6668da2d")
set(HERMES_PACKAGE_NAME "Microsoft.JavaScript.Hermes")
set(HERMES_PACKAGE_DIR "${NUGET_PACKAGES_DIR}/${HERMES_PACKAGE_NAME}.${HERMES_PACKAGE_VERSION}")

# Create custom target to download NuGet package
add_custom_target(download_hermes_package
  COMMAND ${CMAKE_COMMAND} -E make_directory ${NUGET_PACKAGES_DIR}
  COMMAND nuget install ${HERMES_PACKAGE_NAME} -Version ${HERMES_PACKAGE_VERSION} -OutputDirectory ${NUGET_PACKAGES_DIR} -NonInteractive
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Downloading Hermes NuGet package..."
)

# Define paths to Hermes binaries
set(HERMES_LIB_DIR "${HERMES_PACKAGE_DIR}/build/native/win32/x64")
set(HERMES_INCLUDE_DIR "${HERMES_PACKAGE_DIR}/build/native/include")
set(HERMES_LIB "${HERMES_LIB_DIR}/hermes.lib")
set(HERMES_DLL "${HERMES_LIB_DIR}/hermes.dll")

# Create executable
add_executable(hermes-cli
  child_process.cpp
  child_process.h
  compat.h
  node_lite.cpp
  node_lite.h
  node_lite_hermes.cpp
  node_lite_windows.cpp
  string_utils.cpp
  string_utils.h
)

# Add the .def file to export functions
set_target_properties(hermes-cli PROPERTIES
  LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/hermes-cli.def"
)

# Add dependency on NuGet package download
add_dependencies(hermes-cli download_hermes_package)

# Link against Hermes library
target_link_libraries(hermes-cli PRIVATE ${HERMES_LIB})

# Include directories (if headers are available)
if(EXISTS ${HERMES_INCLUDE_DIR})
  target_include_directories(hermes-cli PRIVATE
    ${HERMES_INCLUDE_DIR}/hermes
    ${HERMES_INCLUDE_DIR}/node-api
  )
endif()

# Copy Hermes DLL to output directory
add_custom_command(TARGET hermes-cli POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${HERMES_DLL}
  $<TARGET_FILE_DIR:hermes-cli>
  COMMENT "Copying hermes.dll to output directory"
)

# Set target properties
set_target_properties(hermes-cli PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

# Add compile definitions
target_compile_definitions(hermes-cli PRIVATE
  WIN32_LEAN_AND_MEAN
  NOMINMAX
)

# Print information
message(STATUS "Hermes package directory: ${HERMES_PACKAGE_DIR}")
message(STATUS "Hermes library: ${HERMES_LIB}")
message(STATUS "Hermes DLL: ${HERMES_DLL}")
