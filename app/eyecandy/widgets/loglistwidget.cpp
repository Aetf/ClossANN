#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QListWidgetItem>
#include <QLabel>
#include <QRegExp>
#include <QAction>
#include "widgets/loglistwidget.h"
#include "utils/awesomeiconprovider.h"

LogListWidget::LogListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setMaxLines(100);
    // Allow multiple selections
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    // Context menu
    QAction *copyAct = new QAction(AwesomeIconProvider::instance()->icon(fa::copy), tr("复制"), this);
    QAction *clearAct = new QAction(AwesomeIconProvider::instance()->icon(fa::remove), tr("清除"), this);
    connect(copyAct, SIGNAL(triggered()), SLOT(copySelection()));
    connect(clearAct, SIGNAL(triggered()), SLOT(clearLog()));
    addAction(copyAct);
    addAction(clearAct);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void LogListWidget::setMaxLines(int max)
{
    m_maxLines = max;
}

void LogListWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Copy)) {
        copySelection();
        return;
    }
    if (event->matches(QKeySequence::SelectAll)) {
        selectAll();
        return;
    }
}

void LogListWidget::appendLine(const QString &line)
{
    QListWidgetItem *item = new QListWidgetItem;
    // We need to use QLabel here to support rich text
    QLabel *lbl = new QLabel(line);
    lbl->setContentsMargins(4, 2, 4, 2);
    item->setSizeHint(lbl->sizeHint());
    insertItem(0, item);
    setItemWidget(item, lbl);
    const int nbLines = count();
    // Limit log size
    if (nbLines > m_maxLines)
        delete takeItem(nbLines - 1);
}

void LogListWidget::copySelection()
{
    static QRegExp html_tag("<[^>]+>");
    QList<QListWidgetItem*> items = selectedItems();
    QStringList strings;
    foreach (QListWidgetItem* it, items)
        strings << static_cast<QLabel*>(itemWidget(it))->text().replace(html_tag, "");

    QApplication::clipboard()->setText(strings.join("\n"));
}

void LogListWidget::clearLog() {
    clear();
}
