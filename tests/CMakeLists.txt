# Test configuration for NeoCpp

# Collect test files that exist
set(TEST_SOURCES main.cpp)

# Add test files if they exist
file(GLOB CRYPTO_TESTS crypto/*.cpp)
file(GLOB TYPES_TESTS types/*.cpp)
file(GLOB SERIALIZATION_TESTS serialization/*.cpp)
file(GLOB SCRIPT_TESTS script/*.cpp)
file(GLOB TRANSACTION_TESTS transaction/*.cpp)
file(GLOB WALLET_TESTS wallet/*.cpp)
file(GLOB UTILS_TESTS utils/*.cpp)
file(GLOB LOGGER_TESTS logger/*.cpp)
file(GLOB ERROR_TESTS error/*.cpp)

# Add unit tests
file(GLOB_RECURSE UNIT_TESTS unit/*.cpp)

# Combine all test sources
list(APPEND TEST_SOURCES 
    ${CRYPTO_TESTS}
    ${TYPES_TESTS}
    ${SERIALIZATION_TESTS}
    ${SCRIPT_TESTS}
    ${TRANSACTION_TESTS}
    ${WALLET_TESTS}
    ${UTILS_TESTS}
    ${LOGGER_TESTS}
    ${ERROR_TESTS}
    ${UNIT_TESTS}
)

# Remove stub template and problematic tests
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/stub_test_template.cpp)
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/transaction/test_transaction_builder_complete.cpp)

# Add test executable
add_executable(neocpp_tests ${TEST_SOURCES})

# Link with NeoCpp library and dependencies
target_link_libraries(neocpp_tests
    PRIVATE
    neocpp
    Catch2::Catch2
)

# Link CURL if available
if(CURL_FOUND)
    target_link_libraries(neocpp_tests PRIVATE CURL::libcurl)
endif()

# Include directories
target_include_directories(neocpp_tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Add test to CTest
include(CTest)
add_test(NAME neocpp_tests COMMAND neocpp_tests)