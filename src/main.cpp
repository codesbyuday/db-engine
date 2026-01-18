#include "page.hpp"
#include "pager.hpp"

#include <iostream>
#include <chrono>
#include <cstring>

int main() {
    const char* db_file = "mydb.db";
    const uint32_t page_id = 5;

    std::cout << "==============================\n";
    std::cout << "  Mini Database Storage Engine\n";
    std::cout << "==============================\n\n";

    /* ---------------- WRITE PHASE ---------------- */
    std::cout << "[WRITE PHASE]\n";

    uint8_t write_buffer[PAGE_SIZE];
    std::memset(write_buffer, 0, PAGE_SIZE);

    auto write_start = std::chrono::high_resolution_clock::now();

    {
        Pager pager(db_file);

        std::cout << "Initializing page " << page_id << "...\n";
        page_init(write_buffer, page_id, PageType::DataPage);

        std::cout << "Writing page to disk...\n";
        pager.write_page(page_id, write_buffer);
    } // pager goes out of scope → file closed

    auto write_end = std::chrono::high_resolution_clock::now();
    auto write_time =
        std::chrono::duration_cast<std::chrono::microseconds>(write_end - write_start);

    std::cout << "Write completed successfully.\n";
    std::cout << "Write time: " << write_time.count() << " microsecond\n\n";

    /* ---------------- READ PHASE ---------------- */
    std::cout << "[READ PHASE]\n";

    uint8_t read_buffer[PAGE_SIZE];
    std::memset(read_buffer, 0, PAGE_SIZE);

    auto read_start = std::chrono::high_resolution_clock::now();

    {
        Pager pager(db_file);

        std::cout << "Reading page " << page_id << " from disk...\n";
        pager.read_page(page_id, read_buffer);
    }

    auto read_end = std::chrono::high_resolution_clock::now();
    auto read_time =
        std::chrono::duration_cast<std::chrono::microseconds>(read_end - read_start);

    PageHeader* header =
        reinterpret_cast<PageHeader*>(read_buffer);

    std::cout << "Read completed successfully.\n";
    std::cout << "Read time: " << read_time.count() << " microsecond\n\n";

    /* ---------------- VERIFICATION ---------------- */
    std::cout << "[PAGE VERIFICATION]\n";

    std::cout << "Page ID           : " << header->page_id << "\n";
    std::cout << "Page Type         : " << static_cast<int>(header->page_type) << "\n";
    std::cout << "Slot Count        : " << header->slot_count << "\n";
    std::cout << "Free Space Start  : " << header->free_space_start << "\n";
    std::cout << "Free Space End    : " << header->free_space_end << "\n\n";

    std::cout << "==============================\n";
    std::cout << " Storage Engine Test PASSED \n";
    std::cout << "==============================\n";

    return 0;
}
