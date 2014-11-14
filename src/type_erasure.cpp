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

// see: C++ Template Techniques 2nd Edition, 8.3
namespace type_erasure3 {
    class shared_deleter_base {
    public:
        shared_deleter_base() {}
        virtual ~shared_deleter_base() {}
        virtual void destroy() = 0;
    };
    
    // Bind the types.
    template <typename T, typename D>
    class shared_deleter: public shared_deleter_base {
        T * object;
        D deleter;
        
    public:
        shared_deleter(T * object, D deleter)
            : object(object), deleter(deleter) {}
        
        virtual void destroy() {
            this->deleter(this->object);
        }
    };
    
    template <typename T>
    class smart_ptr {
        T * object;
        shared_deleter_base * deleter;
        
    public:
        explicit smart_ptr(T * object) : object(object), deleter(nullptr) {}
        
        // Erase the types.
        template <typename D>
        smart_ptr(T * object, D deleter) : object(object) {
            this->deleter = new shared_deleter<T,D>(object, deleter);
        }
        
        ~smart_ptr() {
            if (this->deleter) {
                this->deleter->destroy();
                delete this->deleter;
            }
            else {
                delete this->object;
            }
        }
        
        T * operator ->() const { return this->object; }
    };
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
    { using namespace type_erasure3;
        std::cout << "## type_erasure3" << std::endl;
        struct hoge {
            hoge() { std::cout << "hoge::hoge()" << std::endl; }
            ~hoge() { std::cout << "hoge::~hoge()" << std::endl; }
            void something() const { std::cout << "hoge::something()" << std::endl; }
        };
        {
            smart_ptr<hoge> p(new hoge());
            p->something();
        }
        {
            std::cout << "### Using a custom deleter" << std::endl;
            smart_ptr<hoge> p(new hoge(), [](hoge * ptr){
                delete ptr;
                std::cout << "Deleted using a custom deleter." << std::endl;
            });
            p->something();
        }
    }
    return 0;
}
