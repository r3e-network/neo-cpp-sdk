#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/utils/hex.hpp"
#include <sstream>
#include <array>

using namespace neocpp;

TEST_CASE("binary_writer tests", "[binary_writer]") {
    SECTION("Write basic types") {
        std::ostringstream stream;
        BinaryWriter writer(stream);
        
        // Write various types
        writer.write(uint8_t(0xFF));
        writer.write(uint16_t(0x1234));
        writer.write(uint32_t(0xDEADBEEF));
        writer.write(uint64_t(0x123456789ABCDEF0));
        
        std::string result = stream.str();
        Bytes bytes(result.begin(), result.end());
        
        // Verify little-endian encoding
        REQUIRE(bytes[0] == 0xFF);
        REQUIRE(bytes[1] == 0x34);
        REQUIRE(bytes[2] == 0x12);
        REQUIRE(bytes[3] == 0xEF);
        REQUIRE(bytes[4] == 0xBE);
        REQUIRE(bytes[5] == 0xAD);
        REQUIRE(bytes[6] == 0xDE);
        REQUIRE(bytes[7] == 0xF0);
        REQUIRE(bytes[8] == 0xDE);
        REQUIRE(bytes[9] == 0xBC);
        REQUIRE(bytes[10] == 0x9A);
        REQUIRE(bytes[11] == 0x78);
        REQUIRE(bytes[12] == 0x56);
        REQUIRE(bytes[13] == 0x34);
        REQUIRE(bytes[14] == 0x12);
    }
    
    SECTION("Write bytes") {
        std::ostringstream stream;
        BinaryWriter writer(stream);
        
        Bytes data = {0x01, 0x02, 0x03, 0x04, 0x05};
        writer.write(data);
        
        std::string result = stream.str();
        Bytes bytes(result.begin(), result.end());
        
        REQUIRE(bytes == data);
    }
    
    SECTION("Write variable length integer") {
        std::ostringstream stream;
        BinaryWriter writer(stream);
        
        // Test various var int sizes
        writer.writeVarInt(0xFC);  // 1 byte
        writer.writeVarInt(0xFD);  // 3 bytes (0xFD + 2 bytes)
        writer.writeVarInt(0xFFFF);  // 3 bytes
        writer.writeVarInt(0x10000);  // 5 bytes (0xFE + 4 bytes)
        writer.writeVarInt(0x100000000);  // 9 bytes (0xFF + 8 bytes)
        
        std::string result = stream.str();
        Bytes bytes(result.begin(), result.end());
        
        size_t offset = 0;
        
        // 0xFC - single byte
        REQUIRE(bytes[offset++] == 0xFC);
        
        // 0xFD - requires 3 bytes
        REQUIRE(bytes[offset++] == 0xFD);
        REQUIRE(bytes[offset++] == 0xFD);
        REQUIRE(bytes[offset++] == 0x00);
        
        // 0xFFFF - requires 3 bytes
        REQUIRE(bytes[offset++] == 0xFD);
        REQUIRE(bytes[offset++] == 0xFF);
        REQUIRE(bytes[offset++] == 0xFF);
        
        // 0x10000 - requires 5 bytes
        REQUIRE(bytes[offset++] == 0xFE);
        REQUIRE(bytes[offset++] == 0x00);
        REQUIRE(bytes[offset++] == 0x00);
        REQUIRE(bytes[offset++] == 0x01);
        REQUIRE(bytes[offset++] == 0x00);
        
        // 0x100000000 - requires 9 bytes
        REQUIRE(bytes[offset++] == 0xFF);
        REQUIRE(bytes[offset++] == 0x00);
        REQUIRE(bytes[offset++] == 0x00);
        REQUIRE(bytes[offset++] == 0x00);
        REQUIRE(bytes[offset++] == 0x00);
        REQUIRE(bytes[offset++] == 0x01);
    }
    
    SECTION("Write variable length string") {
        std::ostringstream stream;
        BinaryWriter writer(stream);
        
        std::string testString = "Hello, Neo!";
        writer.writeVarString(testString);
        
        std::string result = stream.str();
        Bytes bytes(result.begin(), result.end());
        
        // First byte should be the length
        REQUIRE(bytes[0] == testString.length());
        
        // Rest should be the string data
        std::string decoded(bytes.begin() + 1, bytes.begin() + 1 + testString.length());
        REQUIRE(decoded == testString);
    }
    
    SECTION("Write empty string") {
        std::ostringstream stream;
        BinaryWriter writer(stream);
        
        writer.writeVarString("");
        
        std::string result = stream.str();
        Bytes bytes(result.begin(), result.end());
        
        REQUIRE(bytes.size() == 1);
        REQUIRE(bytes[0] == 0);
    }
    
    SECTION("Write boolean") {
        std::ostringstream stream;
        BinaryWriter writer(stream);
        
        writer.write(true);
        writer.write(false);
        
        std::string result = stream.str();
        Bytes bytes(result.begin(), result.end());
        
        REQUIRE(bytes.size() == 2);
        REQUIRE(bytes[0] == 1);
        REQUIRE(bytes[1] == 0);
    }
    
    SECTION("Write signed integers") {
        std::ostringstream stream;
        BinaryWriter writer(stream);
        
        writer.write(int8_t(-1));
        writer.write(int16_t(-1000));
        writer.write(int32_t(-1000000));
        writer.write(int64_t(-1000000000000));
        
        std::string result = stream.str();
        Bytes bytes(result.begin(), result.end());
        
        // Verify two's complement representation
        REQUIRE(bytes[0] == 0xFF);  // -1 as int8
        
        // -1000 as int16 (little-endian)
        REQUIRE(bytes[1] == 0x18);
        REQUIRE(bytes[2] == 0xFC);
    }
    
    SECTION("Write array with variable length") {
        std::ostringstream stream;
        BinaryWriter writer(stream);
        
        std::vector<uint32_t> array = {1, 2, 3, 4, 5};
        
        // Write array length as var int
        writer.writeVarInt(array.size());
        
        // Write array elements
        for (uint32_t value : array) {
            writer.write(value);
        }
        
        std::string result = stream.str();
        Bytes bytes(result.begin(), result.end());
        
        // First byte is array length
        REQUIRE(bytes[0] == 5);
        
        // Check first element (little-endian uint32)
        REQUIRE(bytes[1] == 0x01);
        REQUIRE(bytes[2] == 0x00);
        REQUIRE(bytes[3] == 0x00);
        REQUIRE(bytes[4] == 0x00);
    }
    
    SECTION("Write fixed-size array") {
        std::ostringstream stream;
        BinaryWriter writer(stream);
        
        std::array<uint8_t, 4> fixedArray = {0xDE, 0xAD, 0xBE, 0xEF};
        
        for (uint8_t byte : fixedArray) {
            writer.write(byte);
        }
        
        std::string result = stream.str();
        Bytes bytes(result.begin(), result.end());
        
        REQUIRE(bytes.size() == 4);
        REQUIRE(bytes[0] == 0xDE);
        REQUIRE(bytes[1] == 0xAD);
        REQUIRE(bytes[2] == 0xBE);
        REQUIRE(bytes[3] == 0xEF);
    }
    
    SECTION("Write long string") {
        std::ostringstream stream;
        BinaryWriter writer(stream);
        
        // Create a string longer than 253 bytes
        std::string longString(300, 'A');
        writer.writeVarString(longString);
        
        std::string result = stream.str();
        Bytes bytes(result.begin(), result.end());
        
        // Length should be encoded as multi-byte varint
        REQUIRE(bytes[0] == 0xFD);  // Indicates 2-byte length follows
        REQUIRE(bytes[1] == 0x2C);  // 300 & 0xFF
        REQUIRE(bytes[2] == 0x01);  // 300 >> 8
        
        // Verify string content
        std::string decoded(bytes.begin() + 3, bytes.begin() + 3 + 300);
        REQUIRE(decoded == longString);
    }
}
