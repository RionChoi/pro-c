#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::swap

class MyVector {
private:
    int* data;
    size_t size;

public:
    // Default constructor
    MyVector() : data(nullptr), size(0) {
        std::cout << "Default constructor called.\n";
    }

    // Constructor with size
    explicit MyVector(size_t s) : size(s) {
        data = new int[size];
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<int>(i);
        }
        std::cout << "Constructor(size) called. Size: " << size << ". Address: " << data << "\n";
    }

    // Destructor
    ~MyVector() {
        std::cout << "Destructor called. Size: " << size << ". Address: " << data << "\n";
        delete[] data;
        data = nullptr; // Prevent dangling pointer
        size = 0;
    }

    // Copy constructor
    MyVector(const MyVector& other) : size(other.size) {
        data = new int[size];
        std::copy(other.data, other.data + other.size, data);
        std::cout << "Copy constructor called. Size: " << size << ". Address: " << data << " (Copied from " << other.data << ")\n";
    }

    // Copy assignment operator
    MyVector& operator=(const MyVector& other) {
        std::cout << "Copy assignment operator called. Current Size: " << size << ". New Size: " << other.size << "\n";
        if (this == &other) { // Self-assignment check
            return *this;
        }
        // Free existing resources
        delete[] data;
        // Allocate new resources and copy
        size = other.size;
        data = new int[size];
        std::copy(other.data, other.data + other.size, data);
        std::cout << "Copy assignment finished. Address: " << data << "\n";
        return *this;
    }

    // Move constructor
    MyVector(MyVector&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
        std::cout << "Move constructor called. Size: " << size << ". Address: " << data << " (Moved from " << other.data << " -> original was nullptr)\n";
    }

    // Move assignment operator
    MyVector& operator=(MyVector&& other) noexcept {
        std::cout << "Move assignment operator called. Current Size: " << size << ". New Size: " << other.size << "\n";
        if (this == &other) { // Self-assignment check for move
            return *this;
        }
        delete[] data; // Free existing resources

        data = other.data;
        size = other.size;

        other.data = nullptr;
        other.size = 0;

        std::cout << "Move assignment finished. Address: " << data << "\n";
        return *this;
    }

    // Getter for size
    size_t getSize() const {
        return size;
    }

    // Print elements
    void printElements() const {
        std::cout << "Elements: [";
        for (size_t i = 0; i < size; ++i) {
            std::cout << data[i] << (i == size - 1 ? "" : ", ");
        }
        std::cout << "]\n";
    }
};

MyVector createMyVector(size_t s) {
    std::cout << "Inside createMyVector function.\n";
    MyVector temp(s);
    std::cout << "Returning from createMyVector.\n";
    return temp; // RVO/NRVO or move constructor will be used here
}

int main() {
    std::cout << "--- Demonstrating Copy Semantics ---\n";
    MyVector v1(5);
    MyVector v2 = v1; // Copy constructor
    v1.printElements();
    v2.printElements();

    MyVector v3(2);
    v3 = v1; // Copy assignment operator
    v3.printElements();

    std::cout << "\n--- Demonstrating Move Semantics ---\n";
    MyVector v4;
    std::cout << "Assigning temporary object to v4 (move assignment expected)...\n";
    v4 = createMyVector(7); // Move assignment operator (or RVO/NRVO)
    v4.printElements();

    MyVector v5 = createMyVector(3); // Move constructor (or RVO/NRVO)
    v5.printElements();

    MyVector v6(10);
    std::cout << "Moving v6 to v7 (move constructor expected)...\n";
    MyVector v7 = std::move(v6);
    if (v6.getSize() == 0) {
        std::cout << "v6 is now in a valid but unspecified state (moved-from state).\n";
    }
    v7.printElements();

    MyVector v8(1);
    std::cout << "Moving v7 to v8 (move assignment expected)...\n";
    v8 = std::move(v7);
    v8.printElements();
    if (v7.getSize() == 0) {
        std::cout << "v7 is now in a valid but unspecified state (moved-from state).\n";
    }

    std::cout << "\nExiting main. Objects will be destroyed.\n";

    return 0;
}
