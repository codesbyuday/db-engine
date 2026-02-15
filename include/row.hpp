#ifndef ROW_HPP
#define ROW_HPP
#include<cstdint>
#include<string>
extern uint16_t serialize(uint32_t rollNo, const std::string &name, uint32_t marks, uint8_t* out_buffer);
extern void deserialize(uint32_t &rollNo, std::string &name, uint32_t &marks, const uint8_t *buffer);

#endif
