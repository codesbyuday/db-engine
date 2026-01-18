#ifndef PAGER_H
#define PAGER_H

#include<cstdint>
#include<cstdio>
#include "page.hpp"

class Pager
{
private:
    FILE* db_file;

public:
   // constructor: open or create DB file
    explicit Pager(const char* file_name); //explicit keyword prevents implicit type conversions that could occur when a constructor takes a single argument (or arguments with default values).

    // disable copying (important!)
    Pager(const Pager&) = delete;
    Pager& operator=(const Pager&) = delete;
    //page file operation
    void write_page(uint32_t pageId, const uint8_t* buffer); //Here buffer is const because it will reads data from the buffer and write it into disk
    void read_page(uint32_t pageId, uint8_t* buffer); // here buffer is not const bcz it will reads from disk and write into buffer so buffer is modified.

    ~Pager(); //destructor used for closing the file and cleanup
};

#endif