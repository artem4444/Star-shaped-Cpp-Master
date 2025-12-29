#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include "data_structuring.hpp"
#include "slaves_state_struct.hpp"


class StarManager {
public:
    void input_handler(uint8_t slave_id, const std::vector<uint8_t>& buffer);
    SlaveRealTimeData getSlaveData(uint8_t slave_id) const;


private:
    ReadState parser_; //one instance for all slaves

    //map each slave to its SlaveRealTimeData instance
    std::map<uint8_t, SlaveRealTimeData> slave_registry;
};