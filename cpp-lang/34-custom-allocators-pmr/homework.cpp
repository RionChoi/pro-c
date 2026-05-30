#include <iostream>
#include <vector>
#include <string>
#include <cstddef>
#include <memory>

template<typename T>
class SimpleAllocator {
private:
    mutable int allocation_count = 0;
    mutable int deallocation_count = 0;

public:
    using value_type = T;

    SimpleAllocator() = default;

    template<typename U>
    SimpleAllocator(const SimpleAllocator<U>&) {}

    T* allocate(std::size_t n) {
        allocation_count++;
        std::cout << "SimpleAllocator: allocating " << n << " elements (call #"
                  << allocation_count << ")\n";
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) {
        deallocation_count++;
        std::cout << "SimpleAllocator: deallocating " << n << " elements (call #"
                  << deallocation_count << ")\n";
        ::operator delete(p);
    }

    int getAllocationCount() const { return allocation_count; }
    int getDeallocationCount() const { return deallocation_count; }
};

template<typename T>
class TrackingAllocator {
private:
    struct Stats {
        std::size_t total_allocated = 0;
        std::size_t total_deallocated = 0;
        int active_objects = 0;
        int allocation_calls = 0;
        int deallocation_calls = 0;
    };

    static Stats& getStats() {
        static Stats stats;
        return stats;
    }

public:
    using value_type = T;

    TrackingAllocator() = default;

    template<typename U>
    TrackingAllocator(const TrackingAllocator<U>&) {}

    T* allocate(std::size_t n) {
        Stats& stats = getStats();
        stats.allocation_calls++;
        stats.total_allocated += n * sizeof(T);
        stats.active_objects++;

        std::cout << "[TrackingAllocator] Allocating " << n << " x " << sizeof(T)
                  << " bytes (total: " << stats.total_allocated << " bytes)\n";
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) {
        Stats& stats = getStats();
        stats.deallocation_calls++;
        stats.total_deallocated += n * sizeof(T);
        stats.active_objects--;

        std::cout << "[TrackingAllocator] Deallocating " << n << " x " << sizeof(T)
                  << " bytes (total deallocated: " << stats.total_deallocated << " bytes)\n";
        ::operator delete(p);
    }

    static void printStats() {
        const Stats& stats = getStats();
        std::cout << "\n=== TrackingAllocator Statistics ===\n";
        std::cout << "Total allocated: " << stats.total_allocated << " bytes\n";
        std::cout << "Total deallocated: " << stats.total_deallocated << " bytes\n";
        std::cout << "Active allocations: " << stats.active_objects << "\n";
        std::cout << "Allocation calls: " << stats.allocation_calls << "\n";
        std::cout << "Deallocation calls: " << stats.deallocation_calls << "\n";
    }
};

int main() {
    std::cout << "=== SimpleAllocator Demo ===\n";
    {
        std::cout << "\nCreating vector<int> with SimpleAllocator:\n";
        std::vector<int, SimpleAllocator<int>> v;
        v.push_back(10);
        v.push_back(20);
        v.push_back(30);
        std::cout << "Vector size: " << v.size() << ", capacity: " << v.capacity() << "\n";
    }

    std::cout << "\n=== TrackingAllocator Demo ===\n";
    {
        std::cout << "\nCreating vector<double> with TrackingAllocator:\n";
        std::vector<double, TrackingAllocator<double>> values;
        values.push_back(3.14);
        values.push_back(2.71);
        values.push_back(1.41);

        std::cout << "\nCreating string with TrackingAllocator:\n";
        std::basic_string<char, std::char_traits<char>, TrackingAllocator<char>> str;
        str = "Hello, Custom Allocators!";
        std::cout << "String: " << str << "\n";

        std::cout << "\nCreating another vector:\n";
        std::vector<int, TrackingAllocator<int>> numbers;
        for (int i = 0; i < 5; ++i) {
            numbers.push_back(i * 10);
        }

        TrackingAllocator<double>::printStats();
    }

    std::cout << "\n=== Comparison: std::allocator vs Custom ===\n";
    {
        std::cout << "\nUsing std::allocator (default):\n";
        std::vector<int> standard_vec;
        standard_vec = {1, 2, 3, 4, 5};
        std::cout << "Standard vector size: " << standard_vec.size() << "\n";

        std::cout << "\nUsing SimpleAllocator:\n";
        std::vector<int, SimpleAllocator<int>> custom_vec;
        custom_vec = {1, 2, 3, 4, 5};
        std::cout << "Custom vector size: " << custom_vec.size() << "\n";
    }

    return 0;
}
