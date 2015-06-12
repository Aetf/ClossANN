#include "loglistline.h"
#include <QHBoxLayout>
#include <QLabel>

LogListLine::LogListLine(const QString &time, const QString &content, QWidget *parent) : QWidget(parent)
{
    auto timeLabel = new QLabel(time);
    auto contentLabel = new QLabel(content);
}

