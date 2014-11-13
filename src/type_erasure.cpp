#include <iostream>

// see: C++ Template Techniques 2nd Edition, 8.2, fig. 8-10
namespace type_erasure1 {
    class drawable {
        struct vtable {
            using func_type = void(void *);
            func_type * draw;
        };
        
        // Bind T type.
        template <typename T>
        struct vtable_initializer {
            static vtable value;
            
            static void draw(void * object_ptr) {
                static_cast<T *>(object_ptr)->draw();
            }
        };
        
        void * object_ptr;
        vtable * vtable_ptr;
        
    public:
        // Erase T type.
        template <typename T>
        drawable(T & object) : object_ptr(&object), vtable_ptr(&vtable_initializer<T>::value) {}
        
        void draw() const {
            this->vtable_ptr->draw(this->object_ptr);
        }
    };
    
    template <typename T>
    drawable::vtable drawable::vtable_initializer<T>::value = {
        &drawable::vtable_initializer<T>::draw
    };
    
    struct circle {
        void draw() const {
            std::cout << "Draw the circle." << std::endl;
        }
    };
}

// see: C++ Template Techniques 2nd Edition, 8.2, fig. 8-13
namespace type_erasure2 {
    using namespace type_erasure1;
    
    struct square {
        void draw() const {
            std::cout << "Draw the square." << std::endl;
        }
    };
    
    struct triangle {
        void draw() const {
            std::cout << "Draw the triangle." << std::endl;
        }
    };
    
    void draw(drawable a) {
        a.draw();
    }
}

int main() {
    { using namespace type_erasure1;
        std::cout << "## type_erasure1" << std::endl;
        circle c;
        drawable a = c;
        a.draw();
    }
    { using namespace type_erasure2;
        std::cout << "## type_erasure2" << std::endl;
        
        circle c;
        draw(c);
        
        square s;
        draw(s);
        
        triangle t;
        draw(t);
    }
    return 0;
}
