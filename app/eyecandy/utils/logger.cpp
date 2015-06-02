#include "utils/logger.h"

#include <QDateTime>
#include <QDebug>

class LoggerStaticInitializer
{
private:
    Logger *instance;

    friend class Logger;
public:
    LoggerStaticInitializer()
    {
        qRegisterMetaType<Log::Msg>();
        instance = new Logger;
    }
};

Q_GLOBAL_STATIC(LoggerStaticInitializer, loggerStaticInitializer)

namespace Log
{
Msg::Msg() {}

Msg::Msg(int id, MsgType type, const QString &message)
    : id(id)
    , timestamp(QDateTime::currentMSecsSinceEpoch())
    , type(type)
    , message(message)
{ }
}

LogWritter::~LogWritter()
{
    if (!--stream->ref) {
        stream->logger->addMessage(stream->buffer, stream->type);
        delete stream;
    }
}

Logger::Logger()
    : lock(QReadWriteLock::Recursive)
    , msgCounter(0)
{
}

Logger *Logger::instance()
{
    return loggerStaticInitializer->instance;
}

void Logger::addMessage(const QString &message, const Log::MsgType &type)
{
    QWriteLocker locker(&lock);

    Log::Msg temp(msgCounter++, type, message);
    m_messages.push_back(temp);

    if (m_messages.size() >= MAX_LOG_MESSAGES)
        m_messages.pop_front();

    emit newLogMessage(temp);
}

void Logger::info(const QString &message)
{
    instance()->addMessage(message, Log::INFO);
}

void Logger::normal(const QString &message)
{
    instance()->addMessage(message, Log::NORMAL);
}

void Logger::warning(const QString &message)
{
    instance()->addMessage(message, Log::WARNING);
}

void Logger::critical(const QString &message)
{
    instance()->addMessage(message, Log::CRITICAL);
}

LogWritter Logger::info()
{
    return LogWritter(instance(), Log::INFO);
}

LogWritter Logger::normal()
{
    return LogWritter(instance(), Log::NORMAL);
}

LogWritter Logger::warning()
{
    return LogWritter(instance(), Log::WARNING);
}

LogWritter Logger::critical()
{
    return LogWritter(instance(), Log::CRITICAL);
}

QVector<Log::Msg> Logger::getMessages(int lastKnownId) const
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
