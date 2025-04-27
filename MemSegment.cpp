#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include "MemSegment.h" // MemSegment 클래스 헤더

// WordBytes implementation
WordBytes::WordBytes() : word(0) {}
WordBytes::WordBytes(std::uint32_t val) : word(val) {}
WordBytes& WordBytes::operator=(std::uint32_t val) {
    word = val;
    return *this;
}

// MemSegment implementation
MemSegment::MemSegment() : name_("unnamed"), length_(0) {}
MemSegment::MemSegment(const std::string& seg_name) : name_(seg_name), length_(0) {}

WordBytes& MemSegment::operator[](size_t index) {
    if (index > length_) {
        throw std::out_of_range("Index out of range. You can only access existing elements or append at the end.");
    }
    if (index == length_) {
        if (length_ >= data_.size()) {
            size_t new_size = ((length_ / GROWTH_CHUNK) + 1) * GROWTH_CHUNK;
            data_.resize(new_size);
        }
        length_++;
    }
    return data_[index];
}

const WordBytes& MemSegment::operator[](size_t index) const {
    if (index >= length_) {
        throw std::out_of_range("Index out of range");
    }
    return data_[index];
}

size_t MemSegment::GetSize() const {
    return length_;
}

size_t MemSegment::GetCapacity() const {
    return data_.size();
}

const std::string& MemSegment::GetName() const {
    return name_;
}

void MemSegment::SetName(const std::string& segName) {
    name_ = segName;
}

void MemSegment::Print() const {
    std::cout << "Segment '" << name_ << "' (size: " << length_ << " words, allocated: " << GetCapacity() << " words):\n";
    for (size_t i = 0; i < length_; ++i) {
        const WordBytes& wb = data_[i];
        std::cout << "  [" << std::setw(4) << i << "] word = 0x" 
                  << std::hex << std::setfill('0') << std::setw(8) << wb.word << " | bytes = ";
        for (int k = 3; k >= 0; --k) {
            std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) 
                      << (static_cast<unsigned int>(wb.byte[k]) & 0xFF) << " ";
        }
        std::cout << std::dec << std::endl;
    }
}

bool MemSegment::SaveTo(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file '" << filename << "' for writing." << std::endl;
        return false;
    }
    for (size_t i = 0; i < length_; ++i) {
        file << std::hex << std::setw(8) << std::setfill('0') << data_[i].word << std::endl;
    }
    if (file.bad()) {
        std::cerr << "Error: Writing to file '" << filename << "' failed." << std::endl;
        file.close();
        return false;
    }
    file.close();
    return true;
}

// MemSegmentList implementation
MemSegmentList::MemSegmentList() {}
MemSegmentList::~MemSegmentList() {}

void MemSegmentList::AddSegment(MemSegment* segment) {
    if (segment) {
        segments_.push_back(segment);
    }
}

bool MemSegmentList::RemoveSegment(MemSegment* segment) {
    for (auto it = segments_.begin(); it != segments_.end(); ++it) {
        if (*it == segment) {
            segments_.erase(it);
            return true;
        }
    }
    return false;
}

MemSegment* MemSegmentList::operator[](size_t index) {
    if (index < segments_.size()) {
        return segments_[index];
    }
    return nullptr;
}

const MemSegment* MemSegmentList::operator[](size_t index) const {
    if (index < segments_.size()) {
        return segments_[index];
    }
    return nullptr;
}

MemSegment* MemSegmentList::FindSegment(const std::string& name) {
    for (auto seg : segments_) {
        if (seg && seg->GetName() == name) {
            return seg;
        }
    }
    return nullptr;
}

size_t MemSegmentList::GetSize() const {
    return segments_.size();
}

void MemSegmentList::PrintAll() const {
    std::cout << "Memory Store - Total segments: " << segments_.size() << std::endl;
    for (size_t i = 0; i < segments_.size(); ++i) {
        std::cout << "\nSegment #" << i << ": ";
        if (segments_[i]) {
            segments_[i]->Print();
        } else {
            std::cout << "<null segment>\n";
        }
    }
}

void MemSegmentList::Clear() {
    segments_.clear();
}

