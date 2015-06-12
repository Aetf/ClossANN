#ifndef LOGLISTLINE_H
#define LOGLISTLINE_H

#include <QWidget>

class LogListLine : public QWidget
{
    Q_OBJECT

    bool expandable;

public:
    explicit LogListLine(const QString &time, const QString &content, QWidget *parent = 0);

signals:

public slots:
};

#endif // LOGLISTLINE_H
