#include "slaves_state_struct.hpp"
#include <vector>

class ReadState {
public:

    SlaveRealTimeData parse(const std::vector<uint8_t>& buffer);


};