#include "page.hpp"
#include "pager.hpp"
#include "table.hpp"
#include "row.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>

/* ANSI Color Codes */
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"

// struct StudentRow {
//     int roll;
//     char name[32];
//     int marks;
// };
// struct recordPosition{
//     int pageId;
//     uint16_t slotId;
// };

/* Typewriter Effect */
void TypeText(const std::string &text, int delay_ms = 15)
{
    for (char c : text)
    {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
}

/* MYDB Startup Banner */
void PrintBanner()
{
    std::cout << CYAN << BOLD << R"(

 /$$   /$$                                               /$$$$$$$  /$$$$$$$ 
| $$$ | $$                                              | $$__  $$| $$__  $$
| $$$$| $$  /$$$$$$  /$$   /$$ /$$   /$$  /$$$$$$$      | $$  \ $$| $$  \ $$
| $$ $$ $$ /$$__  $$|  $$ /$$/| $$  | $$ /$$_____/      | $$  | $$| $$$$$$$ 
| $$  $$$$| $$$$$$$$ \  $$$$/ | $$  | $$|  $$$$$$       | $$  | $$| $$__  $$
| $$\  $$$| $$_____/  >$$  $$ | $$  | $$ \____  $$      | $$  | $$| $$  \ $$
| $$ \  $$|  $$$$$$$ /$$/\  $$|  $$$$$$/ /$$$$$$$/      | $$$$$$$/| $$$$$$$/
|__/  \__/ \_______/|__/  \__/ \______/ |_______/       |_______/ |_______/ 
                                                                            
                                                                               

)" << RESET;

    std::cout << GREEN << BOLD;
    TypeText("Welcome to NEXUS Database Engine\n");
    std::cout << RESET;

    std::cout << YELLOW;
    TypeText("Developed by UDAY GUPTA\n");
    std::cout << RESET;

    std::cout << BLUE;
    TypeText("Source Code: https://github.com/codesbyuday/db-engine\n");
    std::cout << RESET;

    std::cout << RED;
    TypeText("Type 'help' for commands, or '.exit' to quit.\n");
    std::cout << RESET;

    std::cout << CYAN
              << "------------------------------------------------------\n"
              << RESET;
}


int main() {
    PrintBanner();
    Pager pager("mydb.db");
    Table table(pager);

    uint32_t pageId = 1;
    uint8_t buffer[PAGE_SIZE];
    memset(buffer, 0, PAGE_SIZE);
    page_init(buffer, pageId, PageType::DataPage);
    while(true){
        std::string userInput;
        std::cout << GREEN << BOLD << "db_query> " << RESET;
        std::cout << BOLD;
        std::getline(std::cin, userInput);
        std::cout << RESET;
    

        std::stringstream ss(userInput);
        std::string operation;
        uint32_t rollNo;
        uint32_t marks;
        std::string name;
        ss >> operation ;

        if(operation == ".insert") {
        ss >> rollNo >> name >> marks;
        if(ss.fail()){
            std::cout << RED << "Invalid input format!\n" << RESET;
            continue;
        }
        // StudentRow s;
        // s.roll = rollNo;
        // strncpy(s.name, name.c_str(), sizeof(s.name)-1);
        // s.name[31] = '\0';
        // s.marks = marks;

        // int slotNo = insert_record(buffer, reinterpret_cast<uint8_t*>(&s), sizeof(StudentRow));
        // std::cout<<"The record inserted at " << slotNo<<std::endl;
        // pager.write_page(pageId, buffer);
        uint8_t temp_buf[256];
        uint16_t record_size = serialize(rollNo, name, marks, temp_buf);
        struct recordPosition pos = table.table_insert(temp_buf, record_size);
        std::cout<<"Record Inserted Successfully at pageId: "<<pos.pageId <<" SlotID: "<<pos.slotId;
        continue;
        }
        else if(operation == ".select") {
        //     pager.read_page(pageId, buffer);
        //    std::cout << BOLD << "\nSlot | Roll | Name        | Marks\n"<<RESET;
        //    std::cout << "-----------------------------------\n";

        //     PageHeader* header = reinterpret_cast<PageHeader*>(buffer);
        //     int slot_count = header->slot_count;
        //     if(slot_count == 0){
        //         std::cout<<"No data present: ";
        //     }
        //     for(int i = 0; i<slot_count; i++){
        //         uint8_t out_buffer[256];
        //         uint16_t length;
        //         bool ok = read_record(buffer, i, out_buffer, length);
        //         if(ok){
        //             StudentRow * tempRecord = reinterpret_cast<StudentRow*>(out_buffer);
        //             std::cout << i << "    | "
        //               << tempRecord->roll << "    | "
        //               << tempRecord->name << "      | "
        //               << tempRecord->marks << "\n";
        //         }
        //     }
        //     std::cout << "\n";
        table.table_scan();
        }
        else if(operation == ".delete") break; 
        else if(operation == ".exit") {
            std::cout << RED << "Exiting DB...\n" << RESET;
            break;
        }
        else if(operation == "help"){
             std::cout << CYAN << "\nAvailable Commands:\n"
                      << "  .insert   -> Insert a record\n"
                      << "  .scan     -> Scan all records\n"
                      << "  .select    -> Select the record\n"
                      << "  .delete     -> Dump a page\n"
                      << "  .exit     -> Quit shell\n\n"
                      << RESET;
            continue;
        }
        else{
            std::cout<<"command not supported TYPE"<< RED << BOLD << " help "<<RESET<<"for list of commands\n";
            continue;
        }
    }
}
