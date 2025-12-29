/* event-driven pattern: StarManager class
StarManager will be used by a hardware interface class coppying
Kernel space buffer written by IgH to User space buffer

- calls ReadState class on multiple vectors coming from different Slaves at different times

- creates several SlaveRealTimeData instances: one for each Slave
- publishes SlaveRealTimeData instances via API

- std::vector<uint8_t>& buffer is supposed to be passed by Hardware Interface Module, 
that reads buffer from kernel space


+ timestamp to track when each slave last sent data
*/

#include "Star_Manager.hpp"

#include "data_structuring.hpp"
#include <vector>
#include <chrono>


void StarManager::input_handler(uint8_t slave_id, const std::vector<uint8_t>& buffer){
    //parse() implementation is in data_structuring.cpp
    SlaveRealTimeData result = parser_.parse(buffer);

    // current time in nanoseconds since Unix epoch:
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    result.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

         
    result.slave_position = slave_id;
    result.data_valid= true;

    slave_registry[slave_id] = result;

}


SlaveRealTimeData StarManager::getSlaveData(uint8_t slave_id) const {

    return slave_registry.at(slave_id);
}
