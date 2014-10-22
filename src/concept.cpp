#include <iostream>

// see: C++ Template Techniques 2nd Edition, 7.1
#include <utility>
namespace concept1 {
    struct is_less_than_comparable_impl {
        template <typename T, typename U>
        static auto check(T*, U*) -> decltype(
            std::declval<T>() < std::declval<U>(),
            std::true_type()
        );
        
        template <typename T, typename U>
        static auto check(...) -> std::false_type;
    };
    
    template <typename T, typename U>
    struct is_less_than_comparable
        : decltype(is_less_than_comparable_impl::check<T,U>(nullptr, nullptr)) {};
    
    template <typename T>
    T const & min(T const & a, T const & b) {
        static_assert(is_less_than_comparable<T,T>::value, "T must be less than comparatable");
        return a < b ? a : b;
    }
    
    class X {};
    
#if 0
    // Concept lite
    template <typename T>
    constexpr bool LessThanComparable() {
        return has_less<T>::value;
    }
    
    template <LessThanComparable T>
    T const & min(T const & a, T const & b) {
        return a < b ? a : b;
    }
#endif
}


// see: C++ Template Techniques 2nd Edition, 7.2
#include <cmath>
namespace concept2 {
    inline namespace data {
        class point {
            double x_ = 0;
            double y_ = 0;
        public:
            point() = default;
            point(double x, double y) : x_(x), y_(y) {}
            double x() const { return this->x_; }
            double y() const { return this->y_; }
        };
        
        class JPoint {
            double x = 0;
            double y = 0;
        public:
            JPoint() = default;
            JPoint(double aX, double aY) : x(aX), y(aY) {}
            double getX() const { return this->x; }
            double getY() const { return this->y; }
        };
    }
    
    inline namespace traits {
        template <typename T>
        struct point_traits {
            static double x(T const & p) { return p.x(); }
            static double y(T const & p) { return p.y(); }
        };
        
        template <>
        struct point_traits<JPoint> {
            static double x(JPoint const & p) { return p.getX(); }
            static double y(JPoint const & p) { return p.getY(); }
        };
    }
    
    inline namespace algorithm {
        template <typename Point>
        Point subtract(Point const & a, Point const & b) {
            using traits = point_traits<Point>;
            return {
                traits::x(a) - traits::x(b),
                traits::y(a) - traits::y(b)
            };
        }
        
        template <typename Point>
        double distance(Point const & a, Point const & b) {
            using traits = point_traits<Point>;
            Point const d = subtract(a,b);
            return std::sqrt(
                traits::x(d) * traits::x(d) +
                traits::y(d) * traits::y(d)
            );
        }
    }
}


// see: C++ Template Techniques 2nd Edition, 7.3
#include <algorithm>
namespace concept3 {
    inline namespace data {
        using namespace concept2::data;
        
        template <typename Point>
        class line_segment {
            Point p1_;
            Point p2_;
        public:
            using point_type = Point;
            
            line_segment() = default;
            line_segment(Point const & p1, Point const & p2)
                : p1_(p1), p2_(p2) {}
            
            Point const & p1() const { return this->p1_; }
            Point const & p2() const { return this->p2_; }
        };
    }
    
    inline namespace traits {
        using namespace concept2::traits;
        
        template <typename LineSegment>
        struct line_segment_traits {
            using point_type = typename LineSegment::point_type;
            
            static point_type const & p1(LineSegment const & line) { return line.p1(); }
            static point_type const & p2(LineSegment const & line) { return line.p2(); }
        };
    }
    
    inline namespace tags {
        struct point_category {};
        struct line_segment_category {};
        
        template <typename T>
        struct get_geometory_category;
        
        template <>
        struct get_geometory_category<point> {
            using type = point_category;
        };
        
        template <typename Point>
        struct get_geometory_category<line_segment<Point>> {
            using type = line_segment_category;
        };
    }
    
    inline namespace algorithm {
        template <typename Geometory1, typename Geometory2>
        double distance(Geometory1 const & a, Geometory2 const & b) {
            return distance_impl(a, b,
                typename get_geometory_category<Geometory1>::type(),
                typename get_geometory_category<Geometory2>::type()
            );
        }
        
        template <typename Point>
        double distance_impl(Point const & a, Point const & b,
            point_category,
            point_category
        ) {
            return concept2::distance(a,b);
        }
        
        template <typename Point, typename LineSegment>
        double distance_impl(Point const & a, LineSegment const & b,
            point_category,
            line_segment_category
        ) {
            using traits = line_segment_traits<LineSegment>;
            return std::min(
                distance(a, traits::p1(b)),
                distance(a, traits::p2(b))
            );
        }
        
        template <typename LineSegment, typename Point>
        double distance_impl(LineSegment const & a, Point const & b,
            line_segment_category,
            point_category
        ) {
            return distance(b,a);
        }
    }
}

// see: C++ Template Techniques 2nd Edition, 7.4
#include <type_traits>
namespace concept4 {
    namespace geo {
        class point {
            double x_ = 0;
            double y_ = 0;
        public:
            point() = default;
            point(double x, double y) : x_(x), y_(y) {}
            
            double getX() const { return this->x_; }
            double getY() const { return this->y_; }
        };
        
        template <typename T>
        struct is_point_category {
            constexpr static bool value = false;
        };
        
        template <>
        struct is_point_category<point> {
            constexpr static bool value = true;
        };
        
        template <>
        struct is_point_category<std::pair<double, double>> {
            constexpr static bool value = true;
        };
        
        template <typename T>
        struct point_traits {
            static double getX(T const & p) { return p.getX(); }
            static double getY(T const & p) { return p.getY(); }
        };
        
        template <>
        struct point_traits<std::pair<double, double>> {
            using point_type = std::pair<double, double>;
            
            static double getX(point_type const & p) { return p.first; }
            static double getY(point_type const & p) { return p.second; }
        };
    }
    
    inline namespace data {
        using namespace concept2::data;
    }
    
    inline namespace tags {
        struct point_category {};
        
        template <typename T, typename Enable = void>
        struct get_geometory_category;
        
        template <>
        struct get_geometory_category<point> {
            using type = point_category;
        };
        
        template <typename T>
        struct get_geometory_category<
            T,
            typename std::enable_if<geo::is_point_category<T>::value>::type
        > {
            using type = point_category;
        };
    }
    
    inline namespace traits {
        template <typename T, typename Enable = void>
        struct point_traits {
            static double x(T const & p) { return p.x(); }
            static double y(T const & p) { return p.y(); }
        };
        
        template <typename T>
        struct point_traits<
            T,
            typename std::enable_if<geo::is_point_category<T>::value>::type
        > {
            static double x(T const & p) { return geo::point_traits<T>::getX(p); }
            static double y(T const & p) { return geo::point_traits<T>::getY(p); }
        };
    }
    
    inline namespace algorithm {
        template <typename Point>
        Point subtract(Point const & a, Point const & b) {
            using traits = point_traits<Point>;
            return {
                traits::x(a) - traits::x(b),
                traits::y(a) - traits::y(b)
            };
        }
        
        template <typename Point>
        double distance_impl(Point const & a, Point const & b,
            point_category,
            point_category
        ) {
            using traits = point_traits<Point>;
            Point const d = subtract(a,b);
            return std::sqrt(
                traits::x(d) * traits::x(d) +
                traits::y(d) * traits::y(d)
            );
        }
        
        template <typename Geometory1, typename Geometory2>
        double distance(Geometory1 const & a, Geometory2 const & b) {
            return distance_impl(a, b,
                typename get_geometory_category<Geometory1>::type(),
                typename get_geometory_category<Geometory2>::type()
            );
        }
    }
}

int main() {
    { using namespace concept1;
        std::cout << "## concept1" << std::endl;
        std::cout << min(1,2) << std::endl;
#if 0
        min(X(),X()); // Compile Error
#endif
    }
    { using namespace concept2;
        std::cout << "## concept2" << std::endl;
        std::cout << distance(point(0.0, 0.0), point(3.0, 3.0)) << std::endl;
        std::cout << distance(JPoint(0.0, 0.0), JPoint(3.0, 3.0)) << std::endl;
    }
    { using namespace concept3;
        std::cout << "## concept3" << std::endl;
        point p1(0.0, 0.0);
        point p2(3.0, 3.0);
        line_segment<point> line(point(2.0, 2.0), point(3.0, 3.0));
        std::cout << distance(p1, p2) << std::endl;
        std::cout << distance(p1, line) << std::endl;
        std::cout << distance(line, p2) << std::endl;
    }
    { using namespace concept4;
        std::cout << "## concept4" << std::endl;
        {
            point p1(0.0, 0.0);
            point p2(3.0, 3.0);
            std::cout << distance(p1, p2) << std::endl;
        }
        {
            geo::point p1(0.0, 0.0);
            geo::point p2(3.0, 3.0);
            std::cout << distance(p1, p2) << std::endl;
        }
        {
            std::pair<double, double> p1(0.0, 0.0);
            std::pair<double, double> p2(3.0, 3.0);
            std::cout << distance(p1, p2) << std::endl;
        }
    }
    return 0;
}
