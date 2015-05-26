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
