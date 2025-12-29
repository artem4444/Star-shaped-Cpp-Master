/* Ethercat_Hardware_Interface class:
- knows which slaves exist from slaves_order_ vector
- copies a Slave's data from Kernel Space buffer into 
std::vector<uint8_t>& buffer
- for each slave: calls StarManager::input_handler() to structure 
data from the buffer into slave_registry maps: slave_id, SlaveRealTimeData
*/


