#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QVector>
#include <QReadWriteLock>
#include <QObject>
#include <QTextStream>

const int MAX_LOG_MESSAGES = 1000;

class Logger;
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
    Msg();
    Msg(int id, MsgType type, const QString &message);
    int id;
    qint64 timestamp;
    MsgType type;
    QString message;
};
}
Q_DECLARE_METATYPE(Log::MsgType)
Q_DECLARE_METATYPE(Log::Msg)

class Logger;
class LogWritter {

    struct Stream {
        Stream (Logger *l, Log::MsgType t)
            : logger(l), type(t), ts(&buffer, QIODevice::WriteOnly), ref(1) {}

        Logger *logger;
        Log::MsgType type;
        QString buffer;
        QTextStream ts;
        int ref;
    } *stream;

    friend class Logger;
    LogWritter(Logger *logger, Log::MsgType type) : stream(new Stream(logger, type)) {}

public:
    inline LogWritter(const LogWritter &o) : stream(o.stream) { ++stream->ref; }
    ~LogWritter();

    template<typename T>
    inline LogWritter operator<<(T value) { stream->ts << value; return *this; }
    inline LogWritter operator<<(const char* t) { stream->ts << QString::fromLocal8Bit(t); return *this; }
};

class LoggerStaticInitializer;
class Logger : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Logger)

    friend class LoggerStaticInitializer;

public:

    static Logger* instance();
    static void info(const QString &message);
    static void normal(const QString &message);
    static void warning(const QString &message);
    static void critical(const QString &message);
    static LogWritter info();
    static LogWritter normal();
    static LogWritter warning();
    static LogWritter critical();

    void addMessage(const QString &message, const Log::MsgType &type = Log::NORMAL);

    QVector<Log::Msg> getMessages(int lastKnownId = -1) const;

signals:
    void newLogMessage(const Log::Msg &message);

private:
    Logger();

    QVector<Log::Msg> m_messages;
    mutable QReadWriteLock lock;
    int msgCounter;
};

#endif // LOGGER_H
