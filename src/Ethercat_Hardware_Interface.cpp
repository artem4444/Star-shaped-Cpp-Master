/* Ethercat_Hardware_Interface class:
- knows which slaves exist from slaves_order_ vector
- copies a Slave's data from Kernel Space buffer into 
std::vector<uint8_t>& buffer
- for each slave: calls StarManager::input_handler() to structure 
data from the buffer into slave_registry maps: slave_id, SlaveRealTimeData
*/


Ethercat_Hardware_Interface::Ethercat_Hardware_Interface(
    const std::vector<uint8_t>& slaves_order)
    : slaves_order_(slaves_order) 
    //does same as `slaves_order_ = slaves_order;` more efficient
{
    
}



void read_kernel::Ethercat_Hardware_Interface(std::vector<uint8_t>& buffer){
    
}


void write_kernel::Ethercat_Hardware_Interface(std::vector<uint8_t>& buffer){
    
}