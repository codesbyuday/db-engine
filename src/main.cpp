#include "page.hpp"
#include "pager.hpp"
#include <iostream>
#include <cstring>

struct StudentRow {
    int roll;
    char name[32];
    int marks;
};

int main() {
    const char* db_file = "mydb.db";
    uint32_t page_id = 1;

    uint8_t buffer[PAGE_SIZE];
    memset(buffer, 0, PAGE_SIZE);

    // ---------------- WRITE ----------------
    {
        Pager pager(db_file);
        page_init(buffer, page_id, PageType::DataPage);

        StudentRow s1 = {1, "Uday", 95};
        StudentRow s2 = {2, "Rahul", 88};

        int slot1 = insert_record(buffer,
                                  reinterpret_cast<uint8_t*>(&s1),
                                  sizeof(StudentRow));

        int slot2 = insert_record(buffer,
                                  reinterpret_cast<uint8_t*>(&s2),
                                  sizeof(StudentRow));

        std::cout << "Inserted StudentRow at slots: "
                  << slot1 << ", " << slot2 << "\n";

        pager.write_page(page_id, buffer);
    }

    // ---------------- READ ----------------
    uint8_t read_buffer[PAGE_SIZE];
    memset(read_buffer, 0, PAGE_SIZE);

    {
        Pager pager(db_file);
        pager.read_page(page_id, read_buffer);
    }

    std::cout << "\nReading stored rows...\n";

    for(int i = 0; i < 2; i++) {
        uint8_t out[256];
        uint16_t len = 0;

        if(read_record(read_buffer, i, out, len)) {
            StudentRow* row =
                reinterpret_cast<StudentRow*>(out);

            std::cout << "Slot " << i << " => "
                      << row->roll << ", "
                      << row->name << ", "
                      << row->marks << "\n";
        }
    }

    return 0;
}
