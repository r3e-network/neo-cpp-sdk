cmake_minimum_required(VERSION 3.20)
project(NeoCpp VERSION 1.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Export compile commands for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# Include FetchContent for external dependencies
include(FetchContent)

# Try to find nlohmann/json, if not found, fetch it
find_package(nlohmann_json 3.0 QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found, fetching from GitHub...")
    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
    )
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    set(JSON_Install OFF CACHE INTERNAL "")
    set(JSON_ImplicitConversions OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(json)
    # nlohmann_json creates its own namespace target, no alias needed
else()
    message(STATUS "Found nlohmann_json: ${nlohmann_json_VERSION}")
endif()

# Fetch libcurl for HTTP requests
find_package(CURL QUIET)
if(NOT CURL_FOUND)
    message(WARNING "CURL not found, HTTP functionality will be disabled")
    set(HAVE_CURL FALSE)
else()
    set(HAVE_CURL TRUE)
endif()

# Fetch Catch2 for testing
if(BUILD_TESTS)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
    )
    FetchContent_MakeAvailable(Catch2)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Collect source files
file(GLOB_RECURSE NEOCPP_HEADERS include/neocpp/*.hpp)
file(GLOB_RECURSE NEOCPP_SOURCES src/*.cpp)

# Debug: Print number of source files found
list(LENGTH NEOCPP_SOURCES NUM_SOURCES)
list(LENGTH NEOCPP_HEADERS NUM_HEADERS)
message(STATUS "Found ${NUM_SOURCES} source files and ${NUM_HEADERS} header files")

# Ensure we have source files
if(NUM_SOURCES EQUAL 0)
    message(FATAL_ERROR "No source files found in src/ directory!")
endif()

# Create library
add_library(neocpp ${NEOCPP_SOURCES} ${NEOCPP_HEADERS})

# Set include directories
target_include_directories(neocpp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link libraries
target_link_libraries(neocpp PUBLIC
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Link CURL if available
if(HAVE_CURL)
    target_link_libraries(neocpp PUBLIC CURL::libcurl)
    target_compile_definitions(neocpp PUBLIC HAVE_CURL=1)
endif()

# Link json library - handle both fetched and installed versions
if(nlohmann_json_FOUND)
    target_link_libraries(neocpp PUBLIC nlohmann_json::nlohmann_json)
else()
    # Fetched dependency - use BUILD_INTERFACE generator expression to prevent export
    target_link_libraries(neocpp PRIVATE nlohmann_json::nlohmann_json)
    target_include_directories(neocpp 
        PUBLIC 
            $<BUILD_INTERFACE:${json_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )
endif()

# Set properties
set_target_properties(neocpp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(neocpp PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -fPIC
        -Wno-deprecated-declarations
    )
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
        add_subdirectory(tests)
    endif()
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


# Installation only supported when nlohmann_json is found via find_package
# When fetched, we can't properly export the target
if(nlohmann_json_FOUND)
    # Export package
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/NeoCppConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Export targets
    install(TARGETS neocpp
        EXPORT NeoCppTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
    )

    install(EXPORT NeoCppTargets
        FILE NeoCppTargets.cmake
        NAMESPACE NeoCpp::
        DESTINATION lib/cmake/NeoCpp
    )

    # Install public headers preserving directory structure
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/neocpp
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp"
    )

    configure_file(cmake/NeoCppConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/NeoCppConfig.cmake"
        @ONLY
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NeoCppConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NeoCppConfigVersion.cmake"
        DESTINATION lib/cmake/NeoCpp
    )
else()
    message(STATUS "nlohmann_json was fetched - installation disabled. Install nlohmann_json to enable 'make install'")
endif()

# Print configuration summary
message(STATUS "NeoCpp Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Build shared libs: ${BUILD_SHARED_LIBS}")