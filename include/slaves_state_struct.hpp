#include <cstdint>
#include <vector>

typedef struct SlaveRealTimeData
{
    uint16_t status_word;
    int32_t actual_position;
    int32_t actual_velocity;
    int16_t actual_torque;
    uint8_t mode_display;
    uint16_t error_code;
    uint16_t system_status;
    float motor_temperature;
    uint64_t timestamp;
    uint16_t slave_position;
    bool data_valid;
};