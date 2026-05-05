#include <iostream>
#include <memory> // for std::unique_ptr, std::shared_ptr

class Resource {
public:
    int id;
    Resource(int id_val) : id(id_val) {
        std::cout << "Resource " << id << " created.\n";
    }
    ~Resource() {
        std::cout << "Resource " << id << " destroyed.\n";
    }
    void do_something() const {
        std::cout << "Resource " << id << " is doing something.\n";
    }
};

void process_unique_resource(std::unique_ptr<Resource> res) {
    if (res) {
        res->do_something();
    }
    std::cout << "Unique resource processing finished. Resource will be destroyed here if not moved.\n";
}

void process_shared_resource(std::shared_ptr<Resource> res) {
    if (res) {
        std::cout << "Shared resource processing. Ref count: " << res.use_count() << "\n";
        res->do_something();
    }
    std::cout << "Shared resource processing finished.\n";
}

int main() {
    std::cout << "--- std::unique_ptr Demonstration ---\n";
    // Creating a unique_ptr
    std::unique_ptr<Resource> ptr1 = std::make_unique<Resource>(101);
    if (ptr1) {
        ptr1->do_something();
    }

    // Transferring ownership using std::move
    std::unique_ptr<Resource> ptr2 = std::move(ptr1);
    if (ptr1) { // ptr1 is now null
        std::cout << "ptr1 still holds resource (ERROR).\n";
    }
    if (ptr2) {
        std::cout << "ptr2 now owns the resource.\n";
        ptr2->do_something();
    }

    // Passing unique_ptr to a function (moves ownership)
    std::cout << "Calling process_unique_resource with ptr2...\n";
    process_unique_resource(std::move(ptr2));
    if (ptr2) { // ptr2 is now null after move
        std::cout << "ptr2 still holds resource after function call (ERROR).\n";
    }
    std::cout << "Back in main after process_unique_resource.\n";

    // unique_ptr goes out of scope, resource is destroyed
    std::cout << "\n--- std::shared_ptr Demonstration ---\n";
    // Creating a shared_ptr
    std::shared_ptr<Resource> s_ptr1 = std::make_shared<Resource>(201);
    std::cout << "s_ptr1 ref count: " << s_ptr1.use_count() << "\n";

    // Creating another shared_ptr from s_ptr1 (shares ownership)
    std::shared_ptr<Resource> s_ptr2 = s_ptr1;
    std::cout << "s_ptr1 ref count: " << s_ptr1.use_count() << "\n";
    std::cout << "s_ptr2 ref count: " << s_ptr2.use_count() << "\n";

    // Passing shared_ptr to a function (shares ownership)
    std::cout << "Calling process_shared_resource with s_ptr1...\n";
    process_shared_resource(s_ptr1);
    std::cout << "Back in main after process_shared_resource. s_ptr1 ref count: " << s_ptr1.use_count() << "\n";

    // Creating a third shared_ptr
    std::shared_ptr<Resource> s_ptr3;
    s_ptr3 = s_ptr2;
    std::cout << "s_ptr1 ref count: " << s_ptr1.use_count() << "\n";
    std::cout << "s_ptr2 ref count: " << s_ptr2.use_count() << "\n";
    std::cout << "s_ptr3 ref count: " << s_ptr3.use_count() << "\n";

    // s_ptr3 goes out of scope
    {
        std::shared_ptr<Resource> s_ptr4 = s_ptr1;
        std::cout << "Inside inner scope. s_ptr1 ref count: " << s_ptr1.use_count() << "\n";
    } // s_ptr4 goes out of scope, ref count decreases
    std::cout << "Outside inner scope. s_ptr1 ref count: " << s_ptr1.use_count() << "\n";

    // All shared_ptrs go out of scope, resource is destroyed when ref count reaches 0
    std::cout << "\nExiting main. Shared resources will be destroyed.\n";

    return 0;
}
