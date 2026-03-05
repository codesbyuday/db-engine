#ifndef BPTREE
#define BPTREE
#include<cstdint>
#include"pager.hpp"
#include"meta.hpp"
#include"page.hpp"

struct BPlusNodeHeader{
    uint8_t is_leaf;
    uint16_t key_count;
};

//For calculating the max keys in the leaf node
constexpr size_t leaf_space_overhaed = sizeof(PageHeader) + sizeof(BPlusNodeHeader) + sizeof(uint32_t); 
constexpr size_t leafMaxKeys = (PAGE_SIZE - leaf_space_overhaed)/10;

//for calculating the max keys in the internal node
constexpr size_t internal_space_overhead = sizeof(PageHeader) + sizeof(BPlusNodeHeader);
constexpr size_t internal_max_keys = (PAGE_SIZE - internal_space_overhead - 4)/8;



struct BPlusLeafNode{
    uint32_t keys[leafMaxKeys];
    uint32_t pageIds[leafMaxKeys];
    uint16_t slotIds[leafMaxKeys];
    uint32_t next_leaf;
};

struct BPlusInternalNode{
    uint32_t keys[internal_max_keys];
    uint32_t childPages[internal_max_keys + 1];
};

class BPlusTree{
    private:
        Pager &pager;
        MetaManager &meta;
    public:
        BPlusTree(Pager &p, MetaManager &m);
        bool insert(uint32_t key, uint32_t pageId, uint16_t slotId);
        bool search(uint32_t key, uint32_t &outPageId, uint16_t &outSlotId);
};

#endif