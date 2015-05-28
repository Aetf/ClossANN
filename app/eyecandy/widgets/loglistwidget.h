#ifndef LOGLISTWIDGET_H
#define LOGLISTWIDGET_H

#include <QListWidget>

QT_BEGIN_NAMESPACE
class QKeyEvent;
QT_END_NAMESPACE

class LogListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit LogListWidget(QWidget *parent = 0);

    void setMaxLines(int max);

public slots:
    void appendLine(const QString &line);

protected slots:
    void copySelection();
    void clearLog();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    int m_maxLines;
};

#endif // LOGLISTWIDGET_H
