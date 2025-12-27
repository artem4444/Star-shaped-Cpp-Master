#include "data_structuring.hpp"
#include <cstring>


//UNCOMMENT test assertions IN TEST FILE

//HELPER FUNCTIONS to extract all elements of the vector-buffer:
    //Non-generic approach
//size_t offset is the byte position = index
uint16_t extract_uint16_t(const std::vector <uint8_t>& buffer, size_t offset){
    return static_cast<uint16_t>(buffer[offset]) | //LSB
        (static_cast<uint16_t>(buffer[offset + 1]) << 8); //MSB; shifted to bits 8-15
}   
//Ethercat buffer uses Little-Endian order; uints too
//bitwise OR combines x2 parts of a uint16 value: LSB then MSB

uint8_t extract_uint8_t(const std::vector <uint8_t>& buffer, size_t offset){
    return buffer[offset];
}


int32_t extract_int32_t(const std::vector <uint8_t>& buffer, size_t offset){
    uint32_t unsigned_value = 
        static_cast<uint32_t>(buffer[offset]) |
        (static_cast<uint32_t>(buffer[offset + 1]) << 8) |
        (static_cast<uint32_t>(buffer[offset + 2]) << 16) |
        (static_cast<uint32_t>(buffer[offset + 3]) << 24);
    return static_cast<int32_t>(unsigned_value);
}
//Bit 15 (MSB) in int16_t = sign bit
//casting to signed handles sign

int16_t extract_int16_t(const std::vector <uint8_t>& buffer, size_t offset){
    uint16_t unsigned_value = 
        static_cast<uint16_t>(buffer[offset]) | 
        (static_cast<uint16_t> (buffer[offset + 1]) << 8);
    return static_cast<int16_t>(unsigned_value);
}


float extract_float(const std::vector <uint8_t>& buffer, size_t offset){
    float value;
    //mecpy takes addresses as args
    std::memcpy(&value, &buffer[offset], sizeof(float));
    return value;
}


/* ReadState class:
- takes vector-buffer from a single Slave
- creates instance of SlaveRealTimeData from slaves_state_struct.hpp
- calls helper functions to extract data from vector-buffer bytes into the struct
- returns the populated struct
*/
SlaveRealTimeData ReadState::parse(const std::vector <uint8_t>& buffer) {
    SlaveRealTimeData srt;

    //offset = sum of bytes in previous objects
    srt.status_word = extract_uint16_t(buffer, 0);
    srt.actual_position = extract_int32_t(buffer, 2);
    srt.actual_velocity = extract_int32_t(buffer, 6);
    srt.actual_torque = extract_int16_t(buffer, 10);
    srt.mode_display = extract_uint8_t(buffer, 12);
    srt.error_code = extract_uint16_t(buffer, 13);
    srt.system_status = extract_uint16_t(buffer, 15);
    srt.motor_temperature = extract_float(buffer, 17);
    
    return srt;
}

