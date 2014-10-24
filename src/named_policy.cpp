// see: C++ Template Techniques 2nd Edition, 6.5
#include <iostream>
#include <string>
#include <type_traits>

template <typename Strategy>
struct ownership {
    using type = Strategy;
};

struct reference_count {
    static std::string const value;
};
std::string const reference_count::value = "refarence_count";

struct deep_copy {
    static std::string const value;
};
std::string const deep_copy::value = "deep_copy";

template <bool IsMultiThread>
struct multi_thread {
    constexpr static bool value = IsMultiThread;
};

template <typename>
struct is_ownership_policy {
    constexpr static bool value = false;
};

template <typename Strategy>
struct is_ownership_policy<ownership<Strategy>> {
    constexpr static bool value = true;
};

template <typename>
struct is_multi_thread_policy {
    constexpr static bool value = false;
};

template <bool IsMultiThread>
struct is_multi_thread_policy<multi_thread<IsMultiThread>> {
    constexpr static bool value = true;
};

struct not_found {};

template <
    template <typename> class Pred,
    typename Head,
    typename... Tail
>
struct find_if_impl {
    using type = typename std::conditional<
        Pred<Head>::value,
        Head,
        typename find_if_impl<Pred, Tail...>::type
    >::type;
};

template <template <typename> class Pred, typename Head>
struct find_if_impl<Pred, Head> {
    using type = typename std::conditional<
        Pred<Head>::value,
        Head,
        not_found
    >::type;
};

template <template <typename> class Pred, typename... List>
struct find_if {
    using type = typename find_if_impl<Pred, List...>::type;
};

template <template <typename> class Pred, typename... List>
struct get_required_arg {
    using type = typename find_if<Pred, List...>::type;
    static_assert(!std::is_same<type, not_found>::value, "required policy not found");
};

template <typename Opt, template <typename> class Pred, typename... List>
struct get_optional_arg {
private:
    using result = typename find_if<Pred, List...>::type;
public:
    using type = typename std::conditional<
        !std::is_same<result, not_found>::value,
        result,
        Opt
    >::type;
};

template <typename... Args>
struct smart_ptr {
    using ownership_policy = typename get_required_arg<
        is_ownership_policy,
        Args...
    >::type;
    
    using multi_thread_policy = typename get_optional_arg<
        multi_thread<false>,
        is_multi_thread_policy,
        Args...
    >::type;
};

int main() {
    using sp1 = smart_ptr<multi_thread<true>, ownership<reference_count>>;
    using sp2 = smart_ptr<ownership<deep_copy>, multi_thread<false>>;
    using sp3 = smart_ptr<ownership<reference_count>>;
    using sp4 = smart_ptr<multi_thread<true>>;
    std::cout
        << std::boolalpha
        << "sp1: " << sp1::multi_thread_policy::value << ", " << sp1::ownership_policy::type::value << "\n"
        << "sp2: " << sp2::multi_thread_policy::value << ", " << sp2::ownership_policy::type::value << "\n"
        << "sp3: " << sp3::multi_thread_policy::value << ", " << sp3::ownership_policy::type::value << "\n"
#if 0
        << "sp4: " << sp4::multi_thread_policy::value << ", " << sp4::ownership_policy::type::value << "\n" // Error
#endif
        << std::endl;
    return 0;
}
