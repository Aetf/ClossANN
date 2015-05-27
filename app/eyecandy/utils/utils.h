#ifndef UTILS_H
#define UTILS_H

#include <memory>
using std::unique_ptr;

class Utils
{
public:
    Utils();
};

template<typename T>
unique_ptr<T> make_unique(T *pointer)
{
    return unique_ptr<T>(pointer);
}

template<typename... Args> struct Select {
    template<typename C, typename R>
    static constexpr auto OverloadOf( R (C::*pmf)(Args...) ) -> decltype(pmf) {
        return pmf;
    }
};

unsigned int get_seed();

#endif // UTILS_H
