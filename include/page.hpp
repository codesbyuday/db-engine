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
#pragma pack(pop) //Restore the previous alignment So the rest of your program is unaffected

void page_init(uint8_t* buffer, uint32_t pageId, PageType pageType);
#endif