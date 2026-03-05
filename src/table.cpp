#include "table.hpp"
#include "pager.hpp"
#include "page.hpp"
#include "row.hpp"
#include "meta.hpp"
#include<iostream>

Table::Table(Pager &p) : pager(p){
    pager.read_page(0, metaPage);
    metaInfo = reinterpret_cast<MetaPage*>(metaPage);
}

struct recordPosition Table::table_insert(const uint8_t* record, uint16_t size){
    uint8_t buffer[PAGE_SIZE];
    

    if(metaInfo->total_pages == 1){
        page_init(buffer, metaInfo->total_pages, PageType::DataPage);
        int slot_id = insert_record(buffer, record, size);
        pager.write_page(metaInfo->total_pages, buffer);
        struct recordPosition pos = {0, slot_id}; 
        return pos;
    }

    int last_data_pageId = metaInfo->total_pages - 1;
    pager.read_page(last_data_pageId, buffer);

    struct PageHeader *header = reinterpret_cast<PageHeader*>(buffer);
    while(header->page_type != PageType::DataPage){
        pager.read_page(--last_data_pageId, buffer);
        struct PageHeader *header = reinterpret_cast<PageHeader*>(buffer);
    }

    int slot_id = insert_record(buffer, record, size);
    if(slot_id != -1){
        
        pager.write_page(last_data_pageId, buffer);
        struct recordPosition pos = {last_data_pageId, slot_id};
        return pos;
    }
    else{
        int new_pageId = metaInfo->total_pages;
        page_init(buffer, new_pageId, PageType::DataPage);
        metaInfo->total_pages += 1; //also update the total pageCount in the MetaPage
        slot_id = insert_record(buffer, record, size);
        pager.write_page(new_pageId, buffer);
        pager.write_page(0, metaPage);

        struct recordPosition pos = {new_pageId, slot_id};
        return pos;
    }
}

void Table::table_scan(){
    int page_count = metaInfo->total_pages;
    if(page_count == 1){
        std::cout<<"No Records Found";
        return;
    }
    for(int i = 1; i<page_count; i++){
        uint8_t buffer[PAGE_SIZE];
        pager.read_page(i, buffer);
        struct PageHeader *header = reinterpret_cast<PageHeader*>(buffer);
        if(header->page_type != PageType::DataPage) continue; //if the current page not a dataPage then move to the next page in that way we only check the datapages only

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
