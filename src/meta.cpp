#include"meta.hpp"
#include<cstdint>
#include"page.hpp"
#include"pager.hpp"
#include<string>

MetaManager::MetaManager(Pager &p) : pager(p) {}

void MetaManager::init_if_needed(){

    if(pager.get_page_count() == 0){
    uint8_t buffer[PAGE_SIZE] = {0};
    MetaPage *metapage = reinterpret_cast<MetaPage*>(buffer);
    metapage->verification_code = DB_VERIFIER;
    metapage->version = VERSION;
    metapage->total_pages = 1;
    metapage->rootPage_id = 0;
    pager.write_page(0, buffer);
    }
    else{
        return;
    }
}

void MetaManager::write_meta(const MetaPage &meta){
    uint8_t buffer[PAGE_SIZE] = {0};
    memcpy(buffer, &meta, sizeof(MetaPage));
    pager.write_page(0, buffer);
}

MetaPage MetaManager::read_meta(){
    MetaPage meta;
    uint8_t buffer[PAGE_SIZE];
    pager.read_page(0, buffer);
    memcpy(&meta, buffer, sizeof(MetaPage));
}