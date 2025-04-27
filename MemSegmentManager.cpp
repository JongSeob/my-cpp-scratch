#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>  // uint32_t를 위한 헤더

// Union type definition
union WordBytes {
    std::uint32_t word;
    char byte[4];
    
    // Default constructor
    WordBytes() : word(0) {}
    
    // Constructor with uint32_t
    WordBytes(std::uint32_t val) : word(val) {}
};

// 하나의 메모리 세그먼트를 나타내는 클래스
class MemSegment {
private:
    std::vector<WordBytes> data;
    std::string name;  // 세그먼트 이름 (선택적)

public:
    // 기본 생성자
    MemSegment() : name("unnamed") {}
    
    // 이름으로 초기화하는 생성자
    MemSegment(const std::string& segName) : name(segName) {}
    
    // [] 연산자 오버로딩 - 인덱스로 접근
    WordBytes& operator[](size_t index) {
        // 존재하지 않는 인덱스에 접근하려고 하면 자동 확장
        if (index >= data.size()) {
            data.resize(index + 1);
        }
        return data[index];
    }
    
    // const [] 연산자 오버로딩
    const WordBytes& operator[](size_t index) const {
        return data[index];
    }
    
    // 세그먼트 크기 반환
    size_t size() const {
        return data.size();
    }
    
    // 세그먼트 크기 설정
    void resize(size_t size) {
        data.resize(size);
    }
    
    // 세그먼트 이름 반환
    const std::string& getName() const {
        return name;
    }
    
    // 세그먼트 이름 설정
    void setName(const std::string& segName) {
        name = segName;
    }
    
    // 세그먼트 내용 출력
    void print() const {
        std::cout << "Segment '" << name << "' (size: " << data.size() << " words):\n";
        for (size_t i = 0; i < data.size(); ++i) {
            const WordBytes& wb = data[i];
            std::cout << "  [" << std::setw(4) << i << "] word = 0x" 
                      << std::hex << std::setfill('0') << std::setw(8) << wb.word << " | bytes = ";
            
            // 각 바이트를 16진수로 출력 (MSB부터)
            for (int k = 3; k >= 0; --k) {
                std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) 
                          << (static_cast<unsigned int>(wb.byte[k]) & 0xFF) << " ";
            }
            std::cout << std::dec << std::endl;
        }
    }
};

// 여러 메모리 세그먼트를 관리하는 클래스
class MemSegmentManager {
private:
    std::vector<MemSegment> segments;

public:
    // 기본 생성자
    MemSegmentManager() {}
    
    // 새 세그먼트 추가
    MemSegment& addSegment(const std::string& name = "unnamed") {
        segments.emplace_back(name);
        return segments.back();
    }
    
    // [] 연산자 오버로딩 - 세그먼트 인덱스로 접근
    MemSegment& operator[](size_t index) {
        // 존재하지 않는 인덱스에 접근하려고 하면 자동 확장
        if (index >= segments.size()) {
            segments.resize(index + 1);
        }
        return segments[index];
    }
    
    // const [] 연산자 오버로딩
    const MemSegment& operator[](size_t index) const {
        return segments[index];
    }
    
    // 이름으로 세그먼트 찾기
    MemSegment* findSegment(const std::string& name) {
        for (auto& seg : segments) {
            if (seg.getName() == name) {
                return &seg;
            }
        }
        return nullptr;  // 찾지 못한 경우
    }
    
    // 세그먼트 수 반환
    size_t size() const {
        return segments.size();
    }
    
    // 모든 세그먼트 내용 출력
    void printAll() const {
        std::cout << "Memory Segment Manager - Total segments: " << segments.size() << std::endl;
        for (size_t i = 0; i < segments.size(); ++i) {
            std::cout << "\nSegment #" << i << ": ";
            segments[i].print();
        }
    }
};

int main() {
    // 메모리 세그먼트 매니저 생성
    MemSegmentManager manager;
    
    // 세그먼트 추가
    MemSegment& seg1 = manager.addSegment("CODE");
    seg1.resize(4);  // 4개 워드 크기로 설정
    
    // 세그먼트 데이터 설정
    seg1[0].word = 0x12345678;
    seg1[1].word = 0xAABBCCDD;
    seg1[2].word = 0xFFEEDDCC;
    
    // 바이트 단위로 접근
    seg1[3].byte[0] = 0x11;  // LSB
    seg1[3].byte[1] = 0x22;
    seg1[3].byte[2] = 0x33;
    seg1[3].byte[3] = 0x44;  // MSB
    
    // 다른 세그먼트 추가
    MemSegment& seg2 = manager.addSegment("DATA");
    seg2.resize(2);
    seg2[0].word = 0x99887766;
    seg2[1].word = 0x55443322;
    
    // 인덱스로 세그먼트에 직접 접근
    manager[2].setName("BSS");
    manager[2].resize(1);
    manager[2][0].word = 0xABCDEF01;
    
    // 모든 세그먼트 출력
    manager.printAll();
    
    // 이름으로 세그먼트 찾기
    std::cout << "\nFinding segment by name 'DATA':\n";
    MemSegment* dataSegment = manager.findSegment("DATA");
    if (dataSegment) {
        dataSegment->print();
    }
    
    return 0;
}