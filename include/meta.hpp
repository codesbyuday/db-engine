#ifndef META_HPP
#define META_HPP

#include<cstdint>
#include"pager.hpp"
#define DB_VERIFIER 0X0DB123AB
#define VERSION 1

struct MetaPage{
    uint32_t verification_code;
    uint32_t version;
    uint32_t total_pages;
    uint32_t rootPage_id;
};

class MetaManager{
    private:
        Pager &pager;
    public:
        MetaManager(Pager &p);
        void init_if_needed();
        void write_meta(const MetaPage &meta);
        MetaPage read_meta();
};

#endif