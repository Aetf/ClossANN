#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QVector>
#include <QReadWriteLock>
#include <QObject>
#include <QTextStream>

const int MAX_LOG_MESSAGES = 1000;

namespace Log
{
enum MsgType
{
    INFO,
    NORMAL,
    WARNING,
    CRITICAL //ERROR is defined by libtorrent and results in compiler error
};

struct Msg
{
    Msg(int id = 0, MsgType type = INFO, const QString &message = QStringLiteral(""));
    int id;
    qint64 timestamp;
    MsgType type;
    QString message;
};

class LoggerStaticInitializer;
class LogStorage : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(LogStorage)

    friend class LoggerStaticInitializer;
public:

    static LogStorage* instance();

    void addMessage(const QString &message, const Log::MsgType &type = Log::NORMAL);

    QVector<Log::Msg> getMessages(int lastKnownId = -1) const;

signals:
    void newLogMessage(const Log::Msg &message);

private:
    LogStorage();

    QVector<Log::Msg> m_messages;
    mutable QReadWriteLock lock;
    int msgCounter;
};

class Logger {

    struct Stream {
        Stream (LogStorage *s, Log::MsgType t)
            : store(s), type(t), ts(&buffer, QIODevice::WriteOnly), ref(1) {}

        LogStorage *store;
        Log::MsgType type;
        QString buffer;
        QTextStream ts;
        int ref;
    } *stream;

public:
    inline Logger(LogStorage *storage, Log::MsgType type) : stream(new Stream(storage, type)) {}
    inline Logger(const Logger &o) : stream(o.stream) { ++stream->ref; }
    ~Logger();

    template<typename T>
    inline Logger operator<<(T value) { stream->ts << value; return *this; }
    inline Logger operator<<(const char* t) { stream->ts << QString::fromLocal8Bit(t); return *this; }
};

inline Logger info() { return Logger(LogStorage::instance(), INFO); }
inline Logger normal(){ return Logger(LogStorage::instance(), NORMAL); }
inline Logger warning() { return Logger(LogStorage::instance(), WARNING); }
inline Logger critical() { return Logger(LogStorage::instance(), CRITICAL); }
} // namespace Log
using Logger = Log::Logger;

Q_DECLARE_METATYPE(Log::MsgType)
Q_DECLARE_METATYPE(Log::Msg)

#endif // LOGGER_H
