#include "pager.hpp"
#include<cstdio>
#include<iostream>
#include<cerrno>
#include<stdexcept>
#include<cstring>

Pager::Pager(const char* fileName){
    db_file = fopen(fileName, "rb+");
    if(!db_file){
        db_file = fopen(fileName, "wb+");
    }
    if(!db_file){
        throw std::runtime_error(
            std::string("failed to open database file") + std::strerror(errno)
        );
        
    }
    
}

void Pager::write_page(uint32_t pageId, const uint8_t* buffer){
    long offset = pageId * PAGE_SIZE;
    
    //get current file-size
    //After seeking to SEEK_END, ftell returns the file size in bytes because the file position indicator is placed just after the last byte, not on it.
    //offset: A long int value specifying the number of bytes to move from the reference point . It can be positive (move forward), negative (move backward), or zero.
    if(fseek(db_file, 0, SEEK_END) != 0 ){
        throw std::runtime_error("Fseek Failed {SEEK_END}");
    }

    long fileSize = ftell(db_file);
    if(fileSize < 0){
        throw std::runtime_error("ftell failed");
    }

    if(fileSize < offset){
        int missing_bytes = offset-fileSize;
        int missing_pages = missing_bytes/PAGE_SIZE;

        if(fseek(db_file, fileSize, SEEK_SET)!= 0) 
            throw std::runtime_error("Failed to go to last offset of file using fseek"); //before extending the file it is requires to go to last offset of file 
        

        uint8_t zero_page[PAGE_SIZE] = {0};
        for(int i = 0; i<missing_pages; i++){
            //fwrite return the size_t data type
            size_t written = fwrite(zero_page, 1, PAGE_SIZE, db_file); //here size_t is used to safely represent memory sizes and byte counts and here it is used to store the how many number of elements created
            if(written < PAGE_SIZE)  throw std::runtime_error("Failed to extend the file by initializing new memory");
        }
    }

    if(fseek(db_file, offset, SEEK_SET)) throw std::runtime_error("fseek to offset page is failed");

        size_t written = fwrite(buffer, 1, PAGE_SIZE, db_file);
        if(written < PAGE_SIZE) throw std::runtime_error("failed to write page");

    fflush(db_file);
}

void Pager::read_page(uint32_t pageId, uint8_t* buffer){
    long offset = pageId*PAGE_SIZE;
    if(fseek(db_file, 0, SEEK_END) != 0 ){
        throw std::runtime_error("Fseek Failed {SEEK_END}");
    }

    long fileSize = ftell(db_file);
    if(fileSize < 0){
        throw std::runtime_error("ftell failed");
    }
    if(fileSize<offset+PAGE_SIZE) throw std::runtime_error("Failed: Page not exist to read");

    if(fseek(db_file, offset, SEEK_SET)) throw std::runtime_error("Failed to move on page for reading");

    size_t read = fread(buffer, 1, PAGE_SIZE, db_file);
    if(read < PAGE_SIZE) throw std::runtime_error("Failed to read the page or page does not exist");
}

Pager::~Pager(){
    if(db_file){
    fflush(db_file);
    fclose(db_file);
    db_file = nullptr;
  }
  std::cout<<"File Closed:::.."<<std::endl;
}

int Pager::get_page_count(){
    fseek(db_file, 0, SEEK_END);
    int file_size = ftell(db_file);
    return file_size/PAGE_SIZE;
}