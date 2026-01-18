#include "../include/page.hpp"
#include<iostream>

void page_init(uint8_t* buffer, uint32_t pageId, PageType pageType){
    PageHeader* header = reinterpret_cast<PageHeader*>(buffer); // Map the first bytes of the page buffer to a PageHeader without copying
    header->page_id = pageId;
    header->slot_count = 0;
    header->free_space_start = sizeof(PageHeader);
    header->free_space_end = PAGE_SIZE;
    header->page_type = pageType;

    for(int i = sizeof(PageHeader); i<PAGE_SIZE; i++){
        buffer[i] = 0;
    }
    std::cout<<header->page_id<< " "
    <<header->free_space_start <<" "
    <<header->free_space_end<< " "
    <<static_cast<int>(header->page_type)
    <<header->slot_count<<std::endl;

}
