#include <iostream>
#include <iomanip>
#include <cstdint>  // For uint32_t type
#include <utility>  // For std::pair
#include "MemSegment.h" // MemSegment 클래스 헤더

// Function to test MemSegment functionality
void RunMemSegTest() {
    // Create memory segments
    MemSegment seg1("CODE");
    MemSegment seg2("DATA");
    MemSegment seg3("BSS");

    try {
        // Setup segments - using operator[] to append data
        {
            MemSegment& code = seg1;
            code[0] = 0x12345678;
            code[1] = 0xAABBCCDD;
            code[2] = 0xFFEEDDCC;
            code[3].byte[0] = 0x11;
            code[3].byte[1] = 0x22;
            code[3].byte[2] = 0x33;
            code[3].byte[3] = 0x44;
            code[4] = 0x87654321;
        }

        {
            MemSegment& data = seg2;
            data[0] = 0x99887766;
            data[1] = 0x55443322;
        }

        {
            MemSegment& bss = seg3;
            bss[0] = 0xABCDEF01;
        }

        // Print segment sizes
        std::cout << "Segment sizes:\n";
        std::cout << "CODE segment size: " << seg1.GetSize() << " words (allocated: " << seg1.GetCapacity() << " words)\n";
        std::cout << "DATA segment size: " << seg2.GetSize() << " words (allocated: " << seg2.GetCapacity() << " words)\n";
        std::cout << "BSS segment size: " << seg3.GetSize() << " words (allocated: " << seg3.GetCapacity() << " words)\n\n";

        // Create memory store and add the segments
        MemSegmentList mem_seg_list;
        mem_seg_list.AddSegment(&seg1);
        mem_seg_list.AddSegment(&seg2);
        mem_seg_list.AddSegment(&seg3);

        // Access segments through the store
        std::cout << "Accessing through store:\n";
        {
            MemSegment& code = *mem_seg_list.FindSegment("CODE");
            std::cout << "Found CODE segment: word[0] = 0x" << std::hex 
                      << code[0].word << std::dec << std::endl;
        }

        // Use SetStartBytePosition and GetNextByte
        std::cout << "\nUsing SetStartBytePosition (pair version) and GetNextByte:\n";
        // 1) Set start position using a pair
        seg1.SetStartBytePosition(2, 2); // Start at segment index 2, byte index 2
        // 2.1) SetStartBytePosition with a pair variable
        std::pair<uint32_t, uint32_t> start_position = std::make_pair(2, 2);
        seg1.SetStartBytePosition(start_position); // Start at segment index 2, byte index 2
        // 2.2) SetStartBytePosition with a make_pair
        seg1.SetStartBytePosition(std::make_pair(2, 2)); // Start at segment index 2, byte index 2
        for (int i = 0; i < 4; ++i) {
            uint8_t byte = seg1.GetNextByte();
            std::cout << "Byte " << i << ": 0x" << std::hex << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(byte) << std::dec << std::endl;
        }

        // Print all segments in the store
        std::cout << "\nAll segments in store:\n";
        for (size_t i = 0; i < mem_seg_list.GetSize(); ++i) {
            MemSegment& segment = *mem_seg_list[i];
            segment.Print();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    // Run MemSegment tests
    RunMemSegTest();

    return 0;
}