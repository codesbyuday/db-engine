#include "page.hpp"
#include "pager.hpp"

#include <iostream>
#include <cstring>

int main() {
    const char* db_file = "mydb.db";
    const uint32_t page_id = 1;

    std::cout << "===============================\n";
    std::cout << " Slot Reuse Test (Storage Layer)\n";
    std::cout << "===============================\n\n";

    
    // WRITE PHASE
    
    uint8_t buffer[PAGE_SIZE];
    std::memset(buffer, 0, PAGE_SIZE);

    {
        Pager pager(db_file);

        // Initialize page
        page_init(buffer, page_id, PageType::DataPage);

        // Insert Records A, B, C
        const char* recA = "Record-A";
        const char* recB = "Record-B";
        const char* recC = "Record-C";

        int slotA = insert_record(buffer,
                                  reinterpret_cast<const uint8_t*>(recA),
                                  strlen(recA) + 1);

        int slotB = insert_record(buffer,
                                  reinterpret_cast<const uint8_t*>(recB),
                                  strlen(recB) + 1);

        int slotC = insert_record(buffer,
                                  reinterpret_cast<const uint8_t*>(recC),
                                  strlen(recC) + 1);

        std::cout << "Inserted:\n";
        std::cout << "  A -> slot " << slotA << "\n";
        std::cout << "  B -> slot " << slotB << "\n";
        std::cout << "  C -> slot " << slotC << "\n\n";

        // Delete Record B
        std::cout << "Deleting slot " << slotB << " (Record-B)...\n";
        delete_record(buffer, slotB);

        // Insert Record D (should reuse slotB)
        const char* recD = "Record-D";

        int slotD = insert_record(buffer,
                                  reinterpret_cast<const uint8_t*>(recD),
                                  strlen(recD) + 1);

        std::cout << "Inserted:\n";
        std::cout << "  D -> slot " << slotD << " (should reuse slot "
                  << slotB << ")\n\n";

        // Write page back to disk
        pager.write_page(page_id, buffer);
    }

    // -------------------------------
    // READ PHASE
    // -------------------------------
    std::cout << "\n===============================\n";
    std::cout << " Reopening DB and Reading Page\n";
    std::cout << "===============================\n\n";

    uint8_t read_buffer[PAGE_SIZE];
    std::memset(read_buffer, 0, PAGE_SIZE);

    {
        Pager pager(db_file);
        pager.read_page(page_id, read_buffer);
    }

    const PageHeader* header =
        reinterpret_cast<const PageHeader*>(read_buffer);

    std::cout << "Page slot_count = " << header->slot_count << "\n\n";

    // Read all slots
    std::cout << "Reading all records:\n";

    for (int i = 0; i < header->slot_count; i++) {
        uint8_t out[256];
        uint16_t out_len = 0;

        bool ok = read_record(read_buffer, i, out, out_len);

        if (!ok) {
            std::cout << "  Slot " << i << " -> [DELETED]\n";
        } else {
            std::cout << "  Slot " << i << " -> " << out << "\n";
        }
    }

    std::cout << "\n===============================\n";
    std::cout << " Slot Reuse Test Completed \n";
    std::cout << "===============================\n";

    return 0;
}
