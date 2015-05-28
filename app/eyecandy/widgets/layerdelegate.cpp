#include "layerdelegate.h"
#include "models/layerdescmodel.h"
#include <QComboBox>
#include <QSpinBox>

LayerDelegate::LayerDelegate(QObject *parent)
    : QItemDelegate(parent)
{ }

QWidget *LayerDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &,
                                     const QModelIndex &index) const
{
    switch (index.column()) {
    case LayerDescModel::Col_Type:
    {
        QComboBox *editor = new QComboBox(parent);
        editor->setFrame(false);
        editor->setEditable(false);
        editor->setAutoFillBackground(true);
        populateComboForColType(editor);
        return editor;
    }
    case LayerDescModel::Col_UnitCount:
    {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setFrame(false);
        editor->setMinimum(1);
        editor->setMaximum(1000);
        return editor;
    }
    case LayerDescModel::Col_ActFunc:
    {
        QComboBox *editor = new QComboBox(parent);
        editor->setFrame(false);
        editor->setEditable(false);
        editor->setAutoFillBackground(true);
        populateComboForColActFunc(editor);
        return editor;
    }
    default:
        return nullptr;
    }
}

void LayerDelegate::populateComboForColType(QComboBox *editor) const
{
    editor->clear();
    editor->addItem("FullyConnected", LayerDesc::FullyConnected);
}

void LayerDelegate::populateComboForColActFunc(QComboBox *editor) const
{
    editor->clear();
    editor->addItem("Tanh", LayerDesc::TANH);
    editor->addItem("Logistic", LayerDesc::LOGISTIC);
    editor->addItem("Tanh (scaled)", LayerDesc::TANH_SCALED);
    editor->addItem("Rectifier", LayerDesc::RECTIFIER);
    editor->addItem("Identity", LayerDesc::LINEAR);
}

void LayerDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch (index.column()) {
    case LayerDescModel::Col_Type:
    {
        auto value = index.model()->data(index, Qt::EditRole).toInt();
        auto combo = static_cast<QComboBox*>(editor);
        combo->setCurrentIndex(combo->findData(value));
        break;
    }
    case LayerDescModel::Col_UnitCount:
    {
        auto value = index.model()->data(index, Qt::EditRole).toInt();
        auto spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(value);
        break;
    }
    case LayerDescModel::Col_ActFunc:
    {
        auto value = index.model()->data(index, Qt::EditRole).toInt();
        auto combo = static_cast<QComboBox*>(editor);
        combo->setCurrentIndex(combo->findData(value));
        break;
    }
    default:
        break;
    }

}

void LayerDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    switch (index.column()) {
    case LayerDescModel::Col_Type:
    {
        auto combo = static_cast<QComboBox*>(editor);
        model->setData(index, combo->currentData(), Qt::EditRole);
        break;
    }
    case LayerDescModel::Col_UnitCount:
    {
        auto spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        model->setData(index, spinBox->value(), Qt::EditRole);
        break;
    }
    case LayerDescModel::Col_ActFunc:
    {
        auto combo = static_cast<QComboBox*>(editor);
        model->setData(index, combo->currentData(), Qt::EditRole);
        break;
    }
    default:
        break;
    }
}

void LayerDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
