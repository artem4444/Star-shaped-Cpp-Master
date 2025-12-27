#include "data_structuring.hpp"
#include <cstring>


//UNCOMMENT test assertions IN TEST FILE

//HELPER FUNCTIONS to extract all elements of the vector-buffer:
    //Non-generic approach
//size_t offset is the byte position = index
uint16_t extarct_uint16_t(const std::vector <uint8_t>& buffer, size_t offset){
    return static_cast<uint16_t>(buffer[offset]) | 
        (static_cast<uint16_t>(buffer[offset + 1]) << 8);
}


int32_t

int16_t

uint8_t

float

SlaveRealTimeData ReadState::parse(const std::vector <uint8_t>& buffer) {

}