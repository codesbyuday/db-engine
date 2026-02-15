#ifndef TABLE_H
#define TABLE_H
#include<cstdint>
#include "pager.hpp"

struct recordPosition{
    int pageId;
    int slotId;
};

class Table{
    private: 
        Pager &pager;
    public:
        Table(Pager &p);
        struct recordPosition table_insert(const uint8_t* record, uint16_t size);
        void table_scan();
};


#endif