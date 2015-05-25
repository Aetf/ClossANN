#ifndef LAYERDELEGATE_H
#define LAYERDELEGATE_H

#include <QItemDelegate>

class QComboBox;

class LayerDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit LayerDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;

protected:
    void populateComboForColType(QComboBox *editor) const;
    void populateComboForColActFunc(QComboBox *editor) const;
};

#endif // LAYERDELEGATE_H
