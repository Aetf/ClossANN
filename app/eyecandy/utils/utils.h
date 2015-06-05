#ifndef UTILS_H
#define UTILS_H

#include <memory>
#include <QMetaObject>
#include <QObject>
#include <Eigen/Core>
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

template<typename... Args> struct Select
{
    template<typename C, typename R>
    static constexpr auto OverloadOf( R (C::*pmf)(Args...) ) -> decltype(pmf) {
        return pmf;
    }
};

unsigned int get_seed();

bool fuzzyCompare(double d1, double d2, double gate = 1e-30);

bool match(const Eigen::VectorXd &out, const Eigen::VectorXd &desired);

#endif // UTILS_H
