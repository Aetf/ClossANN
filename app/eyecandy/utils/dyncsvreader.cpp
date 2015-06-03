#include "dyncsvreader.h"
#include "utils/utils.h"
#define CSV_IO_NO_THREAD
#include "csv.h"

namespace csv = io;
using csv::CSVReader;

#define NUMBER1(_)
#define NUMBER2(_) NUMBER1(_) _(1)
#define NUMBER3(_) NUMBER2(_) _(2)
#define NUMBER4(_) NUMBER3(_) _(3)
#define NUMBER5(_) NUMBER4(_) _(4)
#define NUMBER6(_) NUMBER5(_) _(5)
#define NUMBER7(_) NUMBER6(_) _(6)
#define NUMBER8(_) NUMBER7(_) _(7)
#define NUMBER9(_) NUMBER8(_) _(8)

#define COLUMNAT(X) ,columns[X]
#define COLUMNNAME(X) ,#X

namespace CSV
{

#define DynCsvReaderImpl(n) \
class DynCsvReader##n : public DynCsvReader \
{ \
    CSVReader<n> reader; \
public: \
    DynCsvReader##n(const char *file) \
        : reader(file) \
    { \
        reader.set_header("0" NUMBER##n(COLUMNNAME)); \
    } \
    bool readRow(double *columns) override \
    { \
        try { \
            return reader.read_row( \
                    columns[0] \
                    NUMBER##n(COLUMNAT) \
                    ); \
        } catch (csv::error::can_not_open_file) { \
            throw Exceptions::FileNotFound(); \
        } catch (csv::error::line_length_limit_exceeded) {\
            throw Exceptions::LineLengthLimitExceeded(); \
        } \
    } \
};

NUMBER9(DynCsvReaderImpl)

unique_ptr<DynCsvReader> DynCsvReader::getReader(int columnCount, const char *file)
{
    switch (columnCount)
    {
#define CASE(X) \
    case X: \
        return make_unique(new DynCsvReader##X(file));

    NUMBER9(CASE)

#undef CASE
    default:
        return unique_ptr<DynCsvReader>(nullptr);
    }
}

} // namespace CSV
