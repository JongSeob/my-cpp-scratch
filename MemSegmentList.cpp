#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>  // For uint32_t type

// Union type definition
union WordBytes {
    std::uint32_t word;
    char byte[4];
    
    // Default constructor
    WordBytes() : word(0) {}
    
    // Constructor with uint32_t
    WordBytes(std::uint32_t val) : word(val) {}
    
    // Assignment operator for uint32_t
    WordBytes& operator=(std::uint32_t val) {
        word = val;
        return *this;
    }
};

// Class representing a single memory segment
class MemSegment {
private:
    std::vector<WordBytes> data_;
    std::string name_;  // Segment name (optional)
    static const size_t GROWTH_CHUNK = 100;  // Number of elements to add when expanding
    size_t length_;  // Number of valid elements (renamed from highestUsedIndex)

public:
    // Default constructor
    MemSegment() : name_("unnamed"), length_(0) {}
    
    // Constructor with segment name
    MemSegment(const std::string& seg_name) : name_(seg_name), length_(0) {}
    
    // [] operator overloading - access by index
    WordBytes& operator[](size_t index) {
        // Only allow access to valid indices or the next position
        if (index > length_) {
            throw std::out_of_range("Index out of range. You can only access existing elements or append at the end.");
        }
        
        // Auto-expand if accessing the next position
        if (index == length_) {
            // Check if we need to expand the underlying vector
            if (length_ >= data_.size()) {
                // Calculate new size with growth chunk
                size_t new_size = ((length_ / GROWTH_CHUNK) + 1) * GROWTH_CHUNK;
                data_.resize(new_size);
            }
            // Increment length since we're adding a new element
            length_++;
        }
        
        return data_[index];
    }
    
    // const [] operator overloading - only allow access to valid indices
    const WordBytes& operator[](size_t index) const {
        if (index >= length_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }
    
    // Return the number of valid elements
    size_t GetSize() const {
        return length_;
    }
    
    // Return allocated size
    size_t GetCapacity() const {
        return data_.size();
    }
    
    // Get segment name
    const std::string& GetName() const {
        return name_;
    }
    
    // Set segment name
    void SetName(const std::string& segName) {
        name_ = segName;
    }
    
    // Print segment contents
    void Print() const {
        std::cout << "Segment '" << name_ << "' (size: " << length_ << " words, allocated: " << GetCapacity() << " words):\n";
        for (size_t i = 0; i < length_; ++i) {
            const WordBytes& wb = data_[i];
            std::cout << "  [" << std::setw(4) << i << "] word = 0x" 
                      << std::hex << std::setfill('0') << std::setw(8) << wb.word << " | bytes = ";
            
            // Print each byte in hex (from MSB)
            for (int k = 3; k >= 0; --k) {
                std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) 
                          << (static_cast<unsigned int>(wb.byte[k]) & 0xFF) << " ";
            }
            std::cout << std::dec << std::endl;
        }
    }
};

// Class for storing and referencing multiple memory segments (renamed to MemoryStore)
class MemSegmentList {
private:
    std::vector<MemSegment*> segments_;  // Store pointers to segments (not owning them)

public:
    // Default constructor
    MemSegmentList() {}
    
    // Destructor - doesn't delete segments as they are externally managed
    ~MemSegmentList() {}
    
    // Add an existing segment to the store
    void AddSegment(MemSegment* segment) {
        if (segment) {
            segments_.push_back(segment);
        }
    }
    
    // Remove a segment from the store (doesn't delete it)
    bool RemoveSegment(MemSegment* segment) {
        for (auto it = segments_.begin(); it != segments_.end(); ++it) {
            if (*it == segment) {
                segments_.erase(it);
                return true;
            }
        }
        return false;
    }
    
    // [] operator overloading - access by segment index
    MemSegment* operator[](size_t index) {
        if (index < segments_.size()) {
            return segments_[index];
        }
        return nullptr;
    }
    
    // const [] operator overloading
    const MemSegment* operator[](size_t index) const {
        if (index < segments_.size()) {
            return segments_[index];
        }
        return nullptr;
    }
    
    // Find segment by name
    MemSegment* FindSegment(const std::string& name) {
        for (auto seg : segments_) {
            if (seg && seg->GetName() == name) {
                return seg;
            }
        }
        return nullptr;  // Not found
    }
    
    // Return number of segments
    size_t GetSize() const {
        return segments_.size();
    }
    
    // Print all segments
    void PrintAll() const {
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
    
    // Clear the store (doesn't delete segments)
    void Clear() {
        segments_.clear();
    }
};

int main() {
    // Create memory segments externally
    MemSegment seg1("CODE");
    MemSegment seg2("DATA");
    
    try {
        // Setup segments - can only append at the end
        seg1[0] = 0x12345678;  // First element
        seg1[1] = 0xAABBCCDD;  // Append at the end
        seg1[2] = 0xFFEEDDCC;  // Append at the end
        
        // For byte-level access, we still need to use the .byte member
        seg1[3].byte[0] = 0x11;  // LSB
        seg1[3].byte[1] = 0x22;
        seg1[3].byte[2] = 0x33;
        seg1[3].byte[3] = 0x44;  // MSB
        
        // This would throw an exception: seg1[150] = 0x87654321;
        // Instead, we can only append at the end
        seg1[4] = 0x87654321;  // Append at the end
        
        seg2[0] = 0x99887766;
        seg2[1] = 0x55443322;
        
        
        // Print segment sizes
        std::cout << "Segment sizes:\n";
        std::cout << "CODE segment size: " << seg1.GetSize() << " words (allocated: " << seg1.GetCapacity() << " words)\n";
        std::cout << "DATA segment size: " << seg2.GetSize() << " words (allocated: " << seg2.GetCapacity() << " words)\n";
        
        // Create memory store and add the segments
        MemSegmentList store;
        store.AddSegment(&seg1);
        store.AddSegment(&seg2);
        
        // Access segments through the store
        std::cout << "Accessing through store:\n";
        MemSegment* codeSegment = store.FindSegment("CODE");
        if (codeSegment) {
            std::cout << "Found CODE segment: word[0] = 0x" << std::hex 
                      << (*codeSegment)[0].word << std::dec << std::endl;
        }
        
        // Print all segments in the store
        std::cout << "\nAll segments in store:\n";
        store.PrintAll();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}