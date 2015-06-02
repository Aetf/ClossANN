#include "utils/logger.h"

#include <QDateTime>
#include <QDebug>

namespace Log
{
class LoggerStaticInitializer
{
private:
    LogStorage *instance;

    friend class LogStorage;
public:
    LoggerStaticInitializer()
    {
        qRegisterMetaType<Msg>();
        instance = new LogStorage;
    }
};

Q_GLOBAL_STATIC(LoggerStaticInitializer, loggerStaticInitializer)

Msg::Msg(int id, MsgType type, const QString &message)
    : id(id)
    , timestamp(QDateTime::currentMSecsSinceEpoch())
    , type(type)
    , message(message)
{ }

LogStorage::LogStorage()
    : lock(QReadWriteLock::Recursive)
    , msgCounter(0)
{
}

LogStorage *LogStorage::instance()
{
    return loggerStaticInitializer->instance;
}

void LogStorage::addMessage(const QString &message, const Log::MsgType &type)
{
    QWriteLocker locker(&lock);

    Log::Msg temp(msgCounter++, type, message);
    m_messages.push_back(temp);

    if (m_messages.size() >= MAX_LOG_MESSAGES)
        m_messages.pop_front();

    emit newLogMessage(temp);
}

QVector<Log::Msg> LogStorage::getMessages(int lastKnownId) const
{
    QReadLocker locker(&lock);

    int diff = msgCounter - lastKnownId - 1;
    int size = m_messages.size();

    if ((lastKnownId == -1) || (diff >= size))
        return m_messages;

    if (diff <= 0)
        return QVector<Log::Msg>();

    return m_messages.mid(size - diff);
}

Logger::~Logger()
{
    if (!--stream->ref) {
        stream->store->addMessage(stream->buffer, stream->type);
        delete stream;
    }
}

} // namespace Log
