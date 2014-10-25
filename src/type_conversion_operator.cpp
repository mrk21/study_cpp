#include <iostream>

// see: C++ Template Techniques 2nd Edition, 10.3
namespace overload_by_return_value {
    struct object_a { int id; };
    struct object_b { int id; };
    
    auto make_object(int id) {
        struct impl {
            int id;
            operator object_a() const { std::cout << "a" << std::endl; return {id};     }
            operator object_b() const { std::cout << "b" << std::endl; return {id+100}; }
        };
        return impl{id};
    }
}

int main() {
    { using namespace overload_by_return_value;
        object_a a = make_object(1);
        object_b b = make_object(1);
        std::cout << a.id << std::endl;
        std::cout << b.id << std::endl;
    }
    return 0;
}
