#ifndef PAGE_H
#define PAGE_H

#include<cstdint>

#define PAGE_SIZE 4096
enum class PageType : uint8_t {
    DataPage,
    IndexPage,
    MetaDataPage,
    TablePage
};

#pragma pack(push, 1) //Do not add ANY padding between fields
struct PageHeader {
    uint32_t page_id;
    PageType page_type;
    uint16_t slot_count;
    uint16_t free_space_start;
    uint16_t free_space_end;
};
 struct Slot {
    uint16_t offset; //where record starts in a page;
    uint16_t length; // record size (0 = deleted)
 };
 
#pragma pack(pop) //Restore the previous alignment So the rest of your program is unaffected

void page_init(uint8_t* buffer, uint32_t pageId, PageType pageType);

int insert_record(uint8_t* page_buffer, const uint8_t* record_data, uint16_t record_size);

bool read_record(const uint8_t* page_buffer, uint16_t slot_id, uint8_t* out_buffer, uint16_t &out_length);

bool delete_record(uint8_t* page_buffer, uint16_t slot_id);
void compact_page(uint8_t* page_buffer);
#endif