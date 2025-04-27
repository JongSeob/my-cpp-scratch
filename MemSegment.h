#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <string>

// Union type definition
union WordBytes {
    std::uint32_t word;
    char byte[4];

    WordBytes();
    WordBytes(std::uint32_t val);
    WordBytes& operator=(std::uint32_t val);
};

// Class representing a single memory segment
class MemSegment {
private:
    std::vector<WordBytes> data_;
    std::string name_;
    static const size_t GROWTH_CHUNK = 100;
    size_t length_;

public:
    MemSegment();
    MemSegment(const std::string& seg_name);

    WordBytes& operator[](size_t index);
    const WordBytes& operator[](size_t index) const;

    size_t GetSize() const;
    size_t GetCapacity() const;
    const std::string& GetName() const;
    void SetName(const std::string& segName);
    void Print() const;
    bool SaveTo(const std::string& filename) const;
};

// Class for storing and referencing multiple memory segments
class MemSegmentList {
private:
    std::vector<MemSegment*> segments_;

public:
    MemSegmentList();
    ~MemSegmentList();

    void AddSegment(MemSegment* segment);
    bool RemoveSegment(MemSegment* segment);

    MemSegment* operator[](size_t index);
    const MemSegment* operator[](size_t index) const;

    MemSegment* FindSegment(const std::string& name);
    size_t GetSize() const;
    void PrintAll() const;
    void Clear();
};