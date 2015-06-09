#ifndef DYNCSVREADER_H
#define DYNCSVREADER_H

#include <memory>
#include <exception>
using std::unique_ptr;

namespace CSV
{
    namespace Exceptions
    {

    struct Exception : std::exception {};
    struct FileNotFound : Exception {};
    struct LineLengthLimitExceeded : Exception {};

    }

class DynCsvReader
{
public:
    static unique_ptr<DynCsvReader> createReader(int columnCount, const char *file);

    virtual bool readRow(double *columns) = 0;
};

}
#endif // DYNCSVREADER_H
