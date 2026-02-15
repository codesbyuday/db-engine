#include "row.hpp"
#include<cstdint>
#include<string>
#include<cstring>

uint16_t serialize(uint32_t rollNo, const std::string &name, uint32_t marks, uint8_t*out_buffer){
    uint16_t offset = 0;
    memcpy(out_buffer+offset, &rollNo, 4);
    offset+= 4;

    uint16_t len = name.size();
    memcpy(out_buffer+offset, &len, 2);
    offset+=sizeof(len);

    memcpy(out_buffer+offset, name.data(), len);
    offset+=len;

    memcpy(out_buffer+offset, &marks, 4);
    offset+=4;
    return offset;
}

void deserialize(uint32_t &rollNo, std::string &name, uint32_t &marks, const uint8_t* buffer){
    int offset = 0;
    memcpy(&rollNo, buffer+offset, 4);
    offset+=4;

    uint16_t size;
    memcpy(&size, buffer+offset, 2);
    offset+= 2;

    name.assign(reinterpret_cast<const char*>(buffer+offset), size);
    offset+= size;

    memcpy(&marks, buffer+offset, 4);
    
}