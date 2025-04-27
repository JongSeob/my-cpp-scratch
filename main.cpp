#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

union Data {
    int word;       // 32-bit integer
    char byte[4];   // Array of 4 bytes
};

struct MyItem
{
    std::string name;
    std::vector<Data> values; // Use union Data as the type for values
    int length = 0;           // Initialize length to 0

    // Method to add a Data object and update length
    void push_back(const Data& value) {
        values.push_back(value);
        length = values.size(); // Update length
    }
};

// Wrapper class for std::vector
template <typename T>
class VectorWrapper {
private:
    std::vector<T> data;
    int length = 0; // Initialize length to 0

public:
    VectorWrapper() : data(100), length(0) {}
    VectorWrapper(size_t size) : data(size), length(size) {}

    // Overloaded operator() for 1D access
    T& operator()(size_t i) {
        if (length == 0) throw std::out_of_range("Vector is empty.");
        if (i >= length) throw std::out_of_range("Index out of range.");
        return data[i];
    }

    // Overloaded operator() for 2D access
    Data& operator()(size_t i, size_t j) {
        if (length == 0) throw std::out_of_range("Vector is empty.");
        if (i >= length) throw std::out_of_range("Index out of range.");
        if (j >= data[i].length) throw std::out_of_range("Index out of range."); // Use length
        return data[i].values[j];
    }

    // Method to push_back on an element of the vector
    void push_back(size_t i, const Data& value) {
        if (i >= length) throw std::out_of_range("Index out of range.");
        data[i].push_back(value);
        data[i].length = data[i].values.size(); // Update length of MyItem
    }

    // Method to push_back a new element to the vector
    void push_back(const T& value) {
        if (length >= data.size()) throw std::out_of_range("Vector capacity exceeded.");
        data[length] = value;
        length++; // Update length
    }
};

// int main() {
//     std::cout << "Hello, World!" << std::endl;

//     // Create a single instance of VectorWrapper
//     VectorWrapper<MyItem> vector(3); // Initialize with space for 3 MyItem objects

//     // Add data to the first MyItem
//     Data data1;
//     data1.word = 0x12345678; // Store a 32-bit integer
//     vector(0).push_back(data1);

//     Data data2;
//     data2.word = 0xAABBCCDD; // Store a 32-bit integer
//     vector(0).push_back(data2);

//     // Add data to the second MyItem
//     Data data3;
//     data3.word = 0x87654321;
//     vector(1).push_back(data3);

//     // Add data to the third MyItem
//     Data data4;
//     data4.word = 0x11223344;
//     vector(2).push_back(data4);

//     // Access and print data from VectorWrapper
//     std::cout << "First MyItem, first Data (word): 0x" << std::hex << vector(0, 0).word << std::endl;
//     std::cout << "First MyItem, second Data (word): 0x" << std::hex << vector(0, 1).word << std::endl;
//     std::cout << "First MyItem, second Data (byte[3]): 0x" << std::hex << static_cast<unsigned int>(vector(0, 1).byte[3]) << std::endl;
//     std::cout << "Second MyItem, first Data (word): 0x" << std::hex << vector(1, 0).word << std::endl;
//     std::cout << "Third MyItem, first Data (byte[0]): 0x" << std::hex << static_cast<unsigned int>(vector(2, 0).byte[0]) << std::endl;

//     return 0;
// }
