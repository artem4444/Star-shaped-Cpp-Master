#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include <chrono>
#include <thread>
#include <cstring>
#include <limits>
#include "Star_Manager.hpp"
#include "data_structuring.hpp"
#include "slaves_state_struct.hpp"

// Reuse the helper functions from test_data_structuring.cpp
// (You might want to extract these to a common test utilities file)

void append_uint16(std::vector<uint8_t>& buffer, uint16_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
}

void append_int16(std::vector<uint8_t>& buffer, int16_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
}

void append_int32(std::vector<uint8_t>& buffer, int32_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

void append_float(std::vector<uint8_t>& buffer, float value) {
    uint8_t bytes[4];
    std::memcpy(bytes, &value, sizeof(float));
    buffer.insert(buffer.end(), bytes, bytes + 4);
}

std::vector<uint8_t> generate_pdo_buffer(
    uint16_t status_word,
    int32_t actual_position,
    int32_t actual_velocity,
    int16_t actual_torque,
    uint8_t mode_display,
    uint16_t error_code,
    uint16_t system_status,
    float motor_temperature
) {
    std::vector<uint8_t> buffer;
    append_uint16(buffer, status_word);
    append_int32(buffer, actual_position);
    append_int32(buffer, actual_velocity);
    append_int16(buffer, actual_torque);
    buffer.push_back(mode_display);
    append_uint16(buffer, error_code);
    append_uint16(buffer, system_status);
    append_float(buffer, motor_temperature);
    return buffer;
}

// ============================================================================
// TEST FIXTURE
// ============================================================================

class StarManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a standard test buffer
        test_buffer_ = generate_pdo_buffer(
            0x1234,              // status_word
            1000000,             // actual_position
            -50000,              // actual_velocity
            100,                 // actual_torque
            0x08,                // mode_display
            0x0000,              // error_code
            0x00FF,              // system_status
            45.5f                // motor_temperature
        );
        
        // Expected parsed data (without timestamp and slave_position)
        expected_data_.status_word = 0x1234;
        expected_data_.actual_position = 1000000;
        expected_data_.actual_velocity = -50000;
        expected_data_.actual_torque = 100;
        expected_data_.mode_display = 0x08;
        expected_data_.error_code = 0x0000;
        expected_data_.system_status = 0x00FF;
        expected_data_.motor_temperature = 45.5f;
    }
    
    void TearDown() override {
        // Cleanup if needed
    }
    
    std::vector<uint8_t> test_buffer_;
    SlaveRealTimeData expected_data_;
    StarManager manager_;
};

// ============================================================================
// TEST CASE 1: Basic Input Handling
// ============================================================================

TEST_F(StarManagerTest, InputHandlerStoresDataForSlave) {
    const uint8_t slave_id = 1;
    
    // Act: Process input for a slave
    manager_.input_handler(slave_id, test_buffer_);
    
    // Assert: Retrieve and verify the data
    SlaveRealTimeData result = manager_.getSlaveData(slave_id);
    
    // Verify parsed data matches expected values
    EXPECT_EQ(result.status_word, expected_data_.status_word);
    EXPECT_EQ(result.actual_position, expected_data_.actual_position);
    EXPECT_EQ(result.actual_velocity, expected_data_.actual_velocity);
    EXPECT_EQ(result.actual_torque, expected_data_.actual_torque);
    EXPECT_EQ(result.mode_display, expected_data_.mode_display);
    EXPECT_EQ(result.error_code, expected_data_.error_code);
    EXPECT_EQ(result.system_status, expected_data_.system_status);
    EXPECT_FLOAT_EQ(result.motor_temperature, expected_data_.motor_temperature);
    
    // Verify metadata set by input_handler
    EXPECT_EQ(result.slave_position, slave_id);
    EXPECT_TRUE(result.data_valid);
    EXPECT_GT(result.timestamp, 0);  // Timestamp should be set (nanoseconds since epoch)
}

// ============================================================================
// TEST CASE 2: Multiple Slaves
// ============================================================================

TEST_F(StarManagerTest, HandlesMultipleSlaves) {
    // Create different buffers for different slaves
    auto buffer1 = generate_pdo_buffer(0x1234, 1000, 100, 50, 0x08, 0, 0xFF, 40.0f);
    auto buffer2 = generate_pdo_buffer(0x5678, 2000, 200, 75, 0x08, 0, 0xFF, 42.0f);
    auto buffer3 = generate_pdo_buffer(0x9ABC, 3000, 300, 100, 0x08, 0, 0xFF, 44.0f);
    
    // Act: Process inputs for three different slaves
    manager_.input_handler(1, buffer1);
    manager_.input_handler(2, buffer2);
    manager_.input_handler(3, buffer3);
    
    // Assert: Verify each slave's data is stored correctly
    SlaveRealTimeData data1 = manager_.getSlaveData(1);
    SlaveRealTimeData data2 = manager_.getSlaveData(2);
    SlaveRealTimeData data3 = manager_.getSlaveData(3);
    
    EXPECT_EQ(data1.actual_position, 1000);
    EXPECT_EQ(data1.slave_position, 1);
    
    EXPECT_EQ(data2.actual_position, 2000);
    EXPECT_EQ(data2.slave_position, 2);
    
    EXPECT_EQ(data3.actual_position, 3000);
    EXPECT_EQ(data3.slave_position, 3);
}

// ============================================================================
// TEST CASE 3: Update Existing Slave Data
// ============================================================================

TEST_F(StarManagerTest, UpdatesExistingSlaveData) {
    const uint8_t slave_id = 5;
    
    // First update
    auto buffer1 = generate_pdo_buffer(0x1234, 1000, 100, 50, 0x08, 0, 0xFF, 40.0f);
    manager_.input_handler(slave_id, buffer1);
    
    SlaveRealTimeData first = manager_.getSlaveData(slave_id);
    uint64_t first_timestamp = first.timestamp;
    EXPECT_EQ(first.actual_position, 1000);
    
    // Small delay to ensure different timestamp
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    
    // Second update with different data
    auto buffer2 = generate_pdo_buffer(0x5678, 2000, 200, 75, 0x08, 0, 0xFF, 42.0f);
    manager_.input_handler(slave_id, buffer2);
    
    SlaveRealTimeData second = manager_.getSlaveData(slave_id);
    
    // Verify data was updated
    EXPECT_EQ(second.actual_position, 2000);
    EXPECT_EQ(second.status_word, 0x5678);
    EXPECT_GE(second.timestamp, first_timestamp);  // Timestamp should be newer or equal
}

// ============================================================================
// TEST CASE 4: Timestamp Assignment
// ============================================================================

TEST_F(StarManagerTest, AssignsValidTimestamp) {
    const uint8_t slave_id = 1;
    
    // Get current time before processing
    auto before = std::chrono::system_clock::now();
    auto before_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        before.time_since_epoch()).count();
    
    // Act
    manager_.input_handler(slave_id, test_buffer_);
    
    // Get current time after processing
    auto after = std::chrono::system_clock::now();
    auto after_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        after.time_since_epoch()).count();
    
    SlaveRealTimeData result = manager_.getSlaveData(slave_id);
    
    // Assert: Timestamp should be between before and after
    EXPECT_GE(result.timestamp, before_ns);
    EXPECT_LE(result.timestamp, after_ns);
}

// ============================================================================
// TEST CASE 5: Error Handling - Non-existent Slave
// ============================================================================

TEST_F(StarManagerTest, ThrowsExceptionForNonExistentSlave) {
    const uint8_t non_existent_slave_id = 99;
    
    // Assert: Accessing non-existent slave should throw std::out_of_range
    EXPECT_THROW(manager_.getSlaveData(non_existent_slave_id), std::out_of_range);
}

// ============================================================================
// TEST CASE 6: Data Valid Flag
// ============================================================================

TEST_F(StarManagerTest, SetsDataValidFlag) {
    const uint8_t slave_id = 1;
    
    manager_.input_handler(slave_id, test_buffer_);
    SlaveRealTimeData result = manager_.getSlaveData(slave_id);
    
    // Assert: data_valid should always be true after input_handler
    EXPECT_TRUE(result.data_valid);
}

// ============================================================================
// TEST CASE 7: Slave Position Assignment
// ============================================================================

TEST_F(StarManagerTest, SetsCorrectSlavePosition) {
    // Test with different slave IDs
    std::vector<uint8_t> slave_ids = {0, 1, 5, 10, 255};
    
    for (uint8_t slave_id : slave_ids) {
        manager_.input_handler(slave_id, test_buffer_);
        SlaveRealTimeData result = manager_.getSlaveData(slave_id);
        EXPECT_EQ(result.slave_position, slave_id);
    }
}

// ============================================================================
// TEST CASE 8: Edge Case - Zero Slave ID
// ============================================================================

TEST_F(StarManagerTest, HandlesZeroSlaveId) {
    const uint8_t slave_id = 0;
    
    manager_.input_handler(slave_id, test_buffer_);
    SlaveRealTimeData result = manager_.getSlaveData(slave_id);
    
    EXPECT_EQ(result.slave_position, 0);
    EXPECT_TRUE(result.data_valid);
}

// ============================================================================
// TEST CASE 9: Edge Case - Maximum Slave ID
// ============================================================================

TEST_F(StarManagerTest, HandlesMaximumSlaveId) {
    const uint8_t slave_id = UINT8_MAX;  // 255
    
    manager_.input_handler(slave_id, test_buffer_);
    SlaveRealTimeData result = manager_.getSlaveData(slave_id);
    
    EXPECT_EQ(result.slave_position, UINT8_MAX);
    EXPECT_TRUE(result.data_valid);
}

// ============================================================================
// TEST CASE 10: Concurrent Updates (if applicable)
// ============================================================================

TEST_F(StarManagerTest, HandlesRapidUpdates) {
    const uint8_t slave_id = 1;
    
    // Rapidly update the same slave multiple times
    for (int i = 0; i < 10; ++i) {
        auto buffer = generate_pdo_buffer(
            0x1234, 
            1000 + i,  // Changing position
            100, 
            50, 
            0x08, 
            0, 
            0xFF, 
            40.0f
        );
        manager_.input_handler(slave_id, buffer);
    }
    
    SlaveRealTimeData result = manager_.getSlaveData(slave_id);
    EXPECT_EQ(result.actual_position, 1009);  // Last value should be stored
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}