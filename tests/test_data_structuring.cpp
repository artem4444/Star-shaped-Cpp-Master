#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include <cstring>
#include <limits>
#include "data_structuring.hpp"
#include "slaves_state_struct.hpp"

// ============================================================================
// MOCK DATA GENERATION FUNCTIONS
// ============================================================================

/**
 * @brief Helper function to append a 16-bit unsigned integer to buffer (little-endian)
 * EtherCAT uses little-endian byte order, so least significant byte comes first
 */
void append_uint16(std::vector<uint8_t>& buffer, uint16_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));           // LSB (Least Significant Byte)
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));    // MSB (Most Significant Byte)
}

//static_cast
    //int16_t value = 100;
    //int32_t larger = static_cast<int32_t>(value);  // Convert int16 to int32

/**
 * @brief Helper function to append a 16-bit signed integer to buffer (little-endian)
 */
void append_int16(std::vector<uint8_t>& buffer, int16_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
}

/**
 * @brief Helper function to append a 32-bit signed integer to buffer (little-endian)
 */
void append_int32(std::vector<uint8_t>& buffer, int32_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

/**
 * @brief Helper function to append a float to buffer (little-endian)
 * Uses memcpy to preserve exact bit representation
 */
void append_float(std::vector<uint8_t>& buffer, float value) {
    uint8_t bytes[4];
    std::memcpy(bytes, &value, sizeof(float));
    buffer.insert(buffer.end(), bytes, bytes + 4);
}

/**
 * @brief Generate a complete PDO buffer matching the EtherCAT protocol layout
 * This simulates the raw byte stream that would come from the EtherCAT kernel module
 * 
 * @param status_word CiA402 status word (0x6041)
 * @param actual_position Current position in encoder counts (0x6064)
 * @param actual_velocity Current velocity in counts/sec (0x606C)
 * @param actual_torque Current torque/effort (0x6077)
 * @param mode_display Active operation mode (0x6061)
 * @param error_code Custom error code from slave firmware
 * @param system_status Custom system status flags
 * @param motor_temperature Motor temperature in Celsius
 * @return std::vector<uint8_t> Complete PDO buffer in protocol byte order
 */
std::vector<uint8_t> generate_pdo_buffer(
    uint16_t status_word,
    int32_t actual_position,
    int32_t actual_velocity,
    int16_t actual_torque,
    uint8_t mode_display,
    uint16_t error_code,
    uint16_t system_status,
    float motor_temperature
) {  //arguments are provided in SetUp() and other tests
    std::vector<uint8_t> buffer;
    
    // Append fields in the exact order they appear in the struct
    // This matches the byte layout expected by the protocol
    append_uint16(buffer, status_word);                              // 2 bytes: offset 0
    append_int32(buffer, actual_position);                           // 4 bytes: offset 2
    append_int32(buffer, actual_velocity);                           // 4 bytes: offset 6
    append_int16(buffer, actual_torque);                             // 2 bytes: offset 10
    buffer.push_back(mode_display);                                   // 1 byte: offset 12
    append_uint16(buffer, error_code);                                // 2 bytes: offset 13
    append_uint16(buffer, system_status);                             // 2 bytes: offset 15
    append_float(buffer, motor_temperature);                          // 4 bytes: offset 17
    
    // Total: 21 bytes (PDO data only, excluding timestamp, slave_position, data_valid which are metadata)
    
    return buffer;
}

// ============================================================================
// TEST FIXTURE: Sets up common test data and environment
// ============================================================================

/**
 * @brief Test fixture class that provides common setup for all tests
 * This runs before each test, ensuring consistent test environment
 */
class DataStructuringTest : public ::testing::Test {
    //class inherits from Google Test's Test class
protected: // Class AND tests can access
    /**
     * @brief SetUp is called before each test case
     * Initializes test data with known, valid values
     */
    void SetUp() override {
        // Generate a standard test buffer with typical operational values
        // These represent a healthy slave in normal operation
        test_buffer_ = generate_pdo_buffer(
            0x1234,              // status_word: Ready to switch on, Switched on, Operation enabled
            1000000,             // actual_position: 1,000,000 encoder counts
            -50000,              // actual_velocity: -50,000 counts/sec (moving backward)
            100,                 // actual_torque: 100 torque units
            0x08,                // mode_display: Position mode (CiA402)
            0x0000,              // error_code: No errors
            0x00FF,              // system_status: All systems OK
            45.5f                // motor_temperature: 45.5°C (normal operating temp)
        );
        
        // Initialize expected struct values for comparison
        expected_data_.status_word = 0x1234;
        expected_data_.actual_position = 1000000;
        expected_data_.actual_velocity = -50000;
        expected_data_.actual_torque = 100;
        expected_data_.mode_display = 0x08;
        expected_data_.error_code = 0x0000;
        expected_data_.system_status = 0x00FF;
        expected_data_.motor_temperature = 45.5f;
        expected_data_.data_valid = true;
    }
    
    /**
     * @brief TearDown is called after each test case
     * Cleanup if needed (usually not required for simple tests)
     */
    void TearDown() override {
        // Cleanup code here if needed
    }
    
    // Test data available to all test cases
    std::vector<uint8_t> test_buffer_;
    SlaveRealTimeData expected_data_;
};

// ============================================================================
// TEST CASE 1: Basic Functionality - Valid Data Parsing
// ============================================================================

/**
 * @brief Test that valid PDO buffer can be parsed correctly
 * This is the most fundamental test - ensures basic functionality works
 */
TEST_F(DataStructuringTest, ParsesValidPDOBuffer) {
    // Arrange: test_buffer_ is already set up in SetUp()
    
    // Act: Parse the buffer (assuming ReadState has a parse method)
    // ReadState parser;
    // SlaveRealTimeData result = parser.parse(test_buffer_);
    
    // Assert: Verify all fields are extracted correctly
    // EXPECT_EQ(result.status_word, expected_data_.status_word);
    // EXPECT_EQ(result.actual_position, expected_data_.actual_position);
    // EXPECT_EQ(result.actual_velocity, expected_data_.actual_velocity);
    // EXPECT_EQ(result.actual_torque, expected_data_.actual_torque);
    // EXPECT_EQ(result.mode_display, expected_data_.mode_display);
    // EXPECT_EQ(result.error_code, expected_data_.error_code);
    // EXPECT_EQ(result.system_status, expected_data_.system_status);
    // EXPECT_FLOAT_EQ(result.motor_temperature, expected_data_.motor_temperature);
    // EXPECT_TRUE(result.data_valid);
    
    // For now, just verify the buffer was generated correctly
    EXPECT_EQ(test_buffer_.size(), 21);  // Expected buffer size (PDO data only)
    EXPECT_EQ(test_buffer_[0], 0x34);    // LSB of status_word (0x1234)
    EXPECT_EQ(test_buffer_[1], 0x12);    // MSB of status_word
}

// ============================================================================
// TEST CASE 2: Edge Cases - Maximum Values
// ============================================================================

/**
 * @brief Test parsing with maximum possible values
 * Ensures the parser handles boundary conditions correctly
 */
TEST(DataStructuringTest, HandlesMaximumValues) {
    // Generate buffer with maximum values for each field
    auto max_buffer = generate_pdo_buffer(
        UINT16_MAX,                      // Maximum uint16: 65535
        INT32_MAX,                       // Maximum int32: 2147483647
        INT32_MAX,                       // Maximum velocity
        INT16_MAX,                       // Maximum torque: 32767
        UINT8_MAX,                       // Maximum mode: 255
        UINT16_MAX,                      // Maximum error code
        UINT16_MAX,                      // Maximum system status
        std::numeric_limits<float>::max() // Maximum float
    );
    
    // Verify buffer generation succeeded
    EXPECT_EQ(max_buffer.size(), 21);
    //EXPECT_EQ is a Google Test assertion macro that checks if two values are equal
    
    // Test parsing (when implemented)
    // ReadState parser;
    // SlaveRealTimeData result = parser.parse(max_buffer);
    // EXPECT_EQ(result.status_word, UINT16_MAX);
    // EXPECT_EQ(result.actual_position, INT32_MAX);
}

// ============================================================================
// TEST CASE 3: Edge Cases - Minimum Values
// ============================================================================

/**
 * @brief Test parsing with minimum possible values
 * Tests negative numbers and zero values
 */
TEST(DataStructuringTest, HandlesMinimumValues) {
    // Generate buffer with minimum values
    auto min_buffer = generate_pdo_buffer(
        0x0000,                          // Minimum uint16: 0
        INT32_MIN,                       // Minimum int32: -2147483648
        INT32_MIN,                       // Minimum velocity
        INT16_MIN,                       // Minimum torque: -32768
        0x00,                            // Minimum mode: 0
        0x0000,                          // Minimum error code
        0x0000,                          // Minimum system status
        std::numeric_limits<float>::lowest() // Minimum float
    );
    
    EXPECT_EQ(min_buffer.size(), 21);
    
    // Test parsing (when implemented)
    // ReadState parser;
    // SlaveRealTimeData result = parser.parse(min_buffer);
    // EXPECT_EQ(result.actual_position, INT32_MIN);
    // EXPECT_EQ(result.actual_velocity, INT32_MIN);
}

// ============================================================================
// TEST CASE 4: Edge Cases - Zero Values
// ============================================================================

/**
 * @brief Test parsing with all zero values
 * Zero is often a special case that needs explicit testing
 */
TEST(DataStructuringTest, HandlesZeroValues) {
    auto zero_buffer = generate_pdo_buffer(0, 0, 0, 0, 0, 0, 0, 0.0f);
    
    EXPECT_EQ(zero_buffer.size(), 21);
    
    // Verify all bytes are zero (except for float which has specific bit pattern)
    for (size_t i = 0; i < 17; ++i) {  // First 17 bytes should be zero (before float)
        EXPECT_EQ(zero_buffer[i], 0);
    }
}

// ============================================================================
// TEST CASE 5: Error Conditions - Invalid Buffer Size
// ============================================================================

/**
 * @brief Test error handling for buffer that's too small
 * Real hardware might send incomplete data - parser should handle this gracefully
 */
TEST(DataStructuringTest, HandlesInvalidBufferSize) {
    // Create a buffer that's too small
    std::vector<uint8_t> invalid_buffer = {0x01, 0x02, 0x03};  // Only 3 bytes
    
    // Parser should detect this and mark data as invalid
    // ReadState parser;
    // SlaveRealTimeData result = parser.parse(invalid_buffer);
    // EXPECT_FALSE(result.data_valid);
    // EXPECT_THROW(parser.parse(invalid_buffer), std::runtime_error);
    
    // For now, just verify the buffer is indeed too small
    EXPECT_LT(invalid_buffer.size(), 21);
}

// ============================================================================
// TEST CASE 6: Error Conditions - Corrupted Data
// ============================================================================

/**
 * @brief Test handling of corrupted or malformed data
 * In real systems, communication errors can corrupt data
 */
TEST(DataStructuringTest, HandlesCorruptedData) {
    // Create a buffer with correct size but potentially invalid values
    auto corrupted_buffer = generate_pdo_buffer(
        0xFFFF,        // Status word indicating fault
        0x7FFFFFFF,    // Position at maximum (might indicate error)
        0x80000000,    // Velocity at minimum (might indicate error)
        0x7FFF,        // Torque at maximum
        0xFF,          // Invalid mode
        0xFFFF,        // Error code set
        0xFFFF,        // System status indicating faults
        200.0f         // Overheating temperature
    );
    
    EXPECT_EQ(corrupted_buffer.size(), 21);
    
    // Parser should still parse the data, but error flags should be detected
    // ReadState parser;
    // SlaveRealTimeData result = parser.parse(corrupted_buffer);
    // EXPECT_TRUE(result.error_code != 0);  // Error detected
    // EXPECT_GT(result.motor_temperature, 100.0f);  // Overheating detected
}

// ============================================================================
// TEST CASE 7: Real-World Scenarios - Normal Operation
// ============================================================================

/**
 * @brief Test with realistic values from normal operation
 * Uses values that would be seen during typical robot operation
 */
TEST(DataStructuringTest, NormalOperationScenario) {
    // Simulate a joint at 45 degrees (assuming 10000 counts per degree)
    auto normal_buffer = generate_pdo_buffer(
        0x1237,        // Status: Operation enabled, ready
        450000,        // Position: 45 degrees
        1000,          // Velocity: 0.1 deg/sec
        50,            // Torque: 5% of max
        0x08,          // Mode: Position control
        0x0000,        // No errors
        0x0001,        // All systems OK
        42.3f          // Normal temperature
    );
    
    EXPECT_EQ(normal_buffer.size(), 21);
}

// ============================================================================
// TEST CASE 8: Real-World Scenarios - Fault Condition
// ============================================================================

/**
 * @brief Test with values indicating a fault condition
 * Simulates what data looks like when a slave reports an error
 */
TEST(DataStructuringTest, FaultConditionScenario) {
    auto fault_buffer = generate_pdo_buffer(
        0x0008,        // Status: Fault active
        0,             // Position: Unknown/zero
        0,             // Velocity: Stopped
        0,             // Torque: No effort
        0x00,          // Mode: Unknown
        0x2001,        // Error code: Overcurrent fault
        0x8000,        // System status: Fault flag set
        85.0f          // Elevated temperature
    );
    
    EXPECT_EQ(fault_buffer.size(), 21);
}

// ============================================================================
// TEST CASE 9: Byte Order Verification
// ============================================================================

/**
 * @brief Verify that byte order (endianness) is correct
 * EtherCAT uses little-endian, so LSB comes first
 */
TEST(DataStructuringTest, VerifiesLittleEndianByteOrder) {
    // Generate buffer with known value: 0x1234
    auto buffer = generate_pdo_buffer(0x1234, 0, 0, 0, 0, 0, 0, 0.0f);
    
    // In little-endian, 0x1234 should be stored as [0x34, 0x12]
    EXPECT_EQ(buffer[0], 0x34);  // LSB first
    EXPECT_EQ(buffer[1], 0x12);  // MSB second
}

// ============================================================================
// TEST CASE 10: Multiple Slaves Simulation
// ============================================================================

/**
 * @brief Test parsing data from multiple slaves
 * In a real system, you might have multiple slaves, each with their own buffer
 */
TEST(DataStructuringTest, HandlesMultipleSlaves) {
    // Simulate 3 slaves with different data
    std::vector<std::vector<uint8_t>> slave_buffers;
    
    // Slave 1: Joint 1
    slave_buffers.push_back(generate_pdo_buffer(0x1234, 1000, 100, 50, 0x08, 0, 0xFF, 40.0f));
    
    // Slave 2: Joint 2
    slave_buffers.push_back(generate_pdo_buffer(0x1234, 2000, 200, 75, 0x08, 0, 0xFF, 42.0f));
    
    // Slave 3: Joint 3
    slave_buffers.push_back(generate_pdo_buffer(0x1234, 3000, 300, 100, 0x08, 0, 0xFF, 44.0f));
    
    // Verify all buffers are valid
    for (const auto& buffer : slave_buffers) {
        EXPECT_EQ(buffer.size(), 21);
    }
    
    // In real implementation, you would parse each buffer
    // ReadState parser;
    // for (size_t i = 0; i < slave_buffers.size(); ++i) {
    //     SlaveRealTimeData data = parser.parse(slave_buffers[i]);
    //     data.slave_position = i + 1;
    // }
}

// ============================================================================
// MAIN FUNCTION: Google Test Entry Point
// ============================================================================

/**
 * @brief Main function for running all tests
 * Google Test framework handles test discovery and execution
 */
int main(int argc, char **argv) {
    // Initialize Google Test framework
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run all tests and return exit code
    // Returns 0 if all tests pass, non-zero if any test fails
    return RUN_ALL_TESTS();
//RUN_ALL_TESTS() is a Google Test function that runs all registered tests and returns an exit code    
}
