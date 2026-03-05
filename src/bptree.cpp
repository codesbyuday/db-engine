#include"bptree.hpp"


BPlusTree::BPlusTree(Pager &p, MetaManager &m) : pager(p), meta(m) {}

bool BPlusTree::search(uint32_t key, uint32_t &outPageId, uint16_t &outSlotId){
    MetaPage metaPage = meta.read_meta();
    if(metaPage.rootPage_id == 0) return false;
    else{
        uint8_t pageBuffer[PAGE_SIZE]; 
        pager.read_page(metaPage.rootPage_id, pageBuffer);
        BPlusNodeHeader *nodeHeader = reinterpret_cast<BPlusNodeHeader*>(pageBuffer+sizeof(PageHeader));
        while(!(nodeHeader->is_leaf)){
            BPlusInternalNode *internalNode = reinterpret_cast<BPlusInternalNode*>(pageBuffer + sizeof(PageHeader)+sizeof(BPlusNodeHeader));
            for(int i = 0;i<=(nodeHeader->key_count); ++i){
                if(i<(nodeHeader->key_count) && internalNode->keys[i] > key){
                    pager.read_page(internalNode->childPages[i], pageBuffer);
                    nodeHeader = reinterpret_cast<BPlusNodeHeader*>(pageBuffer+sizeof(PageHeader));
                    break;
                }
                else if(i == nodeHeader->key_count){
                    pager.read_page(internalNode->childPages[i], pageBuffer);
                    nodeHeader = reinterpret_cast<BPlusNodeHeader*>(pageBuffer+sizeof(PageHeader));
                }
            }
        }

        //performing operations on Leaf Node(Finding the key on leaf node)
        BPlusLeafNode *leafNode = reinterpret_cast<BPlusLeafNode*>(pageBuffer+ sizeof(PageHeader) + sizeof(BPlusNodeHeader));
        for(int i = 0; i<nodeHeader->key_count; ++i){
            if(leafNode->keys[i] == key){
                outPageId = leafNode->pageIds[i];
                outSlotId = leafNode->slotIds[i];
                return true;
            }
        }
        return false;
    }
}

bool BPlusTree::insert(uint32_t key, uint32_t pageId, uint16_t slotId){
    MetaPage metaPage = meta.read_meta();
    if(metaPage.rootPage_id == 0){
        uint8_t buffer[PAGE_SIZE] = {0};
        metaPage.rootPage_id = metaPage.total_pages; //enter the B+tree root page id into metadata
        
        page_init(buffer, metaPage.total_pages, PageType::IndexPage);
        BPlusNodeHeader *nodeHeader = reinterpret_cast<BPlusNodeHeader*>(buffer+sizeof(PageHeader));
        nodeHeader->is_leaf = true;
        nodeHeader->key_count = 0;
        BPlusLeafNode *leafNode = reinterpret_cast<BPlusLeafNode*>(buffer+sizeof(PageHeader)+sizeof(BPlusNodeHeader));
        leafNode->next_leaf = 0;
        int keyCount = nodeHeader->key_count;
        leafNode->keys[keyCount] = key;
        leafNode->pageIds[keyCount] = pageId;
        leafNode->slotIds[keyCount] = slotId;

        nodeHeader->key_count += 1;
        pager.write_page(metaPage.total_pages, buffer);
        metaPage.total_pages +=1;//after writing rootPage increase the total pages in the meta Info
        meta.write_meta(metaPage); //after performing operation update the meta info specially for updating the root page info
        return true;
    }
}