#include "utils.h"
#include <chrono>

using std::chrono::steady_clock;

Utils::Utils()
{

}

unsigned int get_seed()
{
    auto t = steady_clock::now();
    return t.time_since_epoch().count();
}

bool fuzzyCompare(double d1, double d2, double gate)
{
    return (d1 - d2) <= gate;
}

bool match(const Eigen::VectorXd &out, const Eigen::VectorXd &desired)
{
    if (out.rows() != desired.rows()) return false;

    bool ok = true;
    double gate = (1 - (-1)) / 2.0;
    for (int i = 0; i!= out.rows(); i++) {
        ok = fuzzyCompare(out[i], desired[i], gate);
        if (!ok) break;
    }
    return ok;
}
