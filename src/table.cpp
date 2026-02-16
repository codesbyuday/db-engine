#include "table.hpp"
#include "pager.hpp"
#include "page.hpp"
#include "row.hpp"
#include<iostream>

Table::Table(Pager &p) : pager(p) {}

struct recordPosition Table::table_insert(const uint8_t* record, uint16_t size){
    uint8_t buffer[PAGE_SIZE];
    int pageCount = pager.get_page_count();
    if(pageCount == 0){
        page_init(buffer, 0, PageType::DataPage);
        int slot_id = insert_record(buffer, record, size);
        pager.write_page(0, buffer);
        struct recordPosition pos = {0, slot_id}; 
        return pos;
    }

    int last_pageId = pageCount-1;
    pager.read_page(last_pageId, buffer);
    int slot_id = insert_record(buffer, record, size);
    if(slot_id != -1){
        
        pager.write_page(last_pageId, buffer);
        struct recordPosition pos = {last_pageId, slot_id};
        return pos;
    }
    else{
        int new_pageId = pageCount;
        page_init(buffer, new_pageId, PageType::DataPage);
        slot_id = insert_record(buffer, record, size);
        pager.write_page(new_pageId, buffer);
        struct recordPosition pos = {new_pageId, slot_id};
        return pos;
    }
}

void Table::table_scan(){
    int page_count = pager.get_page_count();
    for(int i = 0; i<page_count; i++){
        uint8_t buffer[PAGE_SIZE];
        pager.read_page(i, buffer);
        struct PageHeader *header = reinterpret_cast<PageHeader*>(buffer);

        for(uint16_t j = 0; j<header->slot_count; j++){
            uint8_t record_buffer[256];
            uint16_t record_length;
            bool ok = read_record(buffer, j, record_buffer, record_length);
            if(!ok) continue;
            uint32_t rollNo, marks;
            std::string name;
            deserialize(rollNo, name, marks, record_buffer);

            // for temporary basis later we can change this and print the data from the shell 
            std::cout << i << " | "
                      << j << " | "
                      << rollNo << " | "
                      << name << " | "
                      << marks << "\n";
        }
    }
}
