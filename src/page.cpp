#include "../include/page.hpp"
#include<iostream>
#include<cstring>

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
    // std::cout<<header->page_id<< " "
    // <<header->free_space_start <<" "
    // <<header->free_space_end<< " "
    // <<static_cast<int>(header->page_type)
    // <<header->slot_count<<std::endl;
}

int insert_record(uint8_t* page_buffer, const uint8_t* record_data, uint16_t record_size){
    PageHeader* header = reinterpret_cast<PageHeader*>(page_buffer);
    uint16_t required = record_size + sizeof(Slot);

    uint16_t available = header->free_space_end - header->free_space_start;

    for(int i = 0; i<header->slot_count; i++){
        Slot* slot = reinterpret_cast<Slot*>((page_buffer+PAGE_SIZE)-((i+1)*sizeof(Slot)));
        if(slot->length == 0){
            if(record_size > available){
                compact_page(page_buffer); //It will remove deleted record
                available = header->free_space_end - header->free_space_start;
                if(record_size>available){return -1;}
            }

            //else write record
            memcpy((page_buffer+(header->free_space_start)), record_data, record_size);
            slot->offset = header->free_space_start;
            header->free_space_start += record_size;
            slot->length = record_size;
            return i;
        }
    }
    
    if(required>available) return -1;

    //write record

    memcpy((page_buffer+(header->free_space_start)), record_data, record_size);
    //create slot

    int slot_id = header->slot_count;
    Slot* slot = reinterpret_cast<Slot*>((page_buffer+header->free_space_end)-sizeof(Slot));
    slot->offset = header->free_space_start;
    slot->length = record_size;

    header->free_space_start += record_size;
    header->free_space_end -= sizeof(struct Slot);
    header->slot_count += 1;

    return slot_id;
}

bool read_record(const uint8_t* page_buffer, uint16_t slot_id, uint8_t* out_buffer, uint16_t &out_length){
   const PageHeader* header = reinterpret_cast<const PageHeader*>(page_buffer);

   //here no need to check to slot_id < 0 bcz it always false as slot_id is uint16_t means unsigned integer
   if(slot_id >= header->slot_count){
    return false;
   }

   const Slot* slot = reinterpret_cast<const Slot*>(page_buffer+PAGE_SIZE-((slot_id+1)*sizeof(Slot)));

   if(slot->length == 0) return false; //check the condition whether a slot is deleted or unavailable

   //It is safety check to ensure data present inside the page limit
   if((slot->offset+slot->length) > PAGE_SIZE) return false;

   //cpy record
   memcpy(out_buffer, &page_buffer[slot->offset], slot->length);
   out_length = slot->length;
   
   return true;
}

bool delete_record(uint8_t * page_buffer, uint16_t slot_id){
    PageHeader* header = reinterpret_cast<PageHeader*>(page_buffer);

    if(slot_id >= header->slot_count) return false;

    //Defensive Check
    if((PAGE_SIZE - (slot_id+1)*sizeof(Slot)) < sizeof(PageHeader)) return false;

    Slot* slot = reinterpret_cast<Slot*>(page_buffer+PAGE_SIZE-((slot_id+1)*sizeof(Slot)));
    if(slot->length == 0) return false;

    slot->length = 0;
    return true;
}

void compact_page(uint8_t *page_buffer){
    PageHeader * header = reinterpret_cast<PageHeader*>(page_buffer);
    uint16_t new_start_space = sizeof(PageHeader); //starts from the record area and move the all deleted space at las by movie moving all active record together
    for(int i = 0; i<header->slot_count; i++){
        Slot *slot = reinterpret_cast<Slot*>(page_buffer+PAGE_SIZE-((i+1)*sizeof(Slot)));
        if(slot->length==0) continue;
        uint16_t old_offset = slot->offset;
        uint16_t old_length = slot->length;
        memcpy(page_buffer+new_start_space, page_buffer+old_offset, old_length);
        slot->offset = new_start_space;
        new_start_space += old_length;
    }
    header->free_space_start = new_start_space;
}

