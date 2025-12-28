/* event-driven pattern
- calls ReadState class on multiple vectors coming from different Slaves at different times

- creates several SlaveRealTimeData instances: one for each Slave
- publishes SlaveRealTimeData instances via API

- std::vector<uint8_t>& buffer is supposed to be passed by Hardware Interface Module, 
that reads buffer from kernel space


+ timestamp to track when each slave last sent data
*/



#include "data_structuring.hpp"
#include <vector>

int main(){

}



int input_handler(uint8_t slave_id, const std::vector<uint8_t>& buffer){
    ReadState parser;
    SlaveRealTimeData result = parser.parse(test_buffer_)

    SlaveManager.timestamp;
    SlaveManager.data_valid = 1;  //flag
}