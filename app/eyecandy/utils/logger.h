#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QVector>
#include <QReadWriteLock>
#include <QObject>

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

class LoggerStaticInitializer;
class Logger : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Logger)

public:
    class LogWritter {
        Logger* logger;
        Log::MsgType type;
        QString content;
        bool empty;

        LogWritter(Logger *logger, Log::MsgType type);
        LogWritter(LogWritter &&writter);

        friend class Logger;
    public:
        ~LogWritter();

        template<typename T>
        LogWritter &operator<<(T value)
        {
            content = QString("%1%2").arg(content).arg(value);
            return *this;
        }
    };

    static Logger* instance();
    static void info(const QString &message);
    static void normal(const QString &message);
    static void warning(const QString &message);
    static void critical(const QString &message);
    static LogWritter info();
    static LogWritter normal();
    static LogWritter warning();
    static LogWritter critical();

    static void drop();
    ~Logger();

    void addMessage(const QString &message, const Log::MsgType &type = Log::NORMAL);

    QVector<Log::Msg> getMessages(int lastKnownId = -1) const;

signals:
    void newLogMessage(const Log::Msg &message);

private:
    friend class LoggerStaticInitializer;
    Logger();
    static Logger* m_instance;
    QVector<Log::Msg> m_messages;
    mutable QReadWriteLock lock;
    int msgCounter;
    int peerCounter;
};

#endif // LOGGER_H
