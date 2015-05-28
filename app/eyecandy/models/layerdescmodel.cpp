#include "layerdescmodel.h"

LayerDescModel::LayerDescModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

LayerDescModel::~LayerDescModel()
{
}

Qt::ItemFlags LayerDescModel::flags(const QModelIndex &index) const
{
    if (!inRange(index)) {
        return QAbstractTableModel::flags(index);
    } else {
        auto normal = Qt::ItemNeverHasChildren | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
        auto editable = normal | Qt::ItemIsEditable;
        switch (index.column()) {
        case Col_Position:
            return normal;
        case Col_ActFunc:
        {
            auto layer = data(index, LayerDataRole).value<LayerDesc>();
            return layer.type == LayerDesc::Input ? normal : editable;
        }
        case Col_UnitCount:
        case Col_Type:
        {
            auto layer = data(index, LayerDataRole).value<LayerDesc>();
            if (layer.type == LayerDesc::Input || layer.type == LayerDesc::Output) {
                return normal;
            } else {
                return editable;
            }
        }
        default:
            return normal;
        }
    }
}

QVariant LayerDescModel::data(const QModelIndex &index, int role) const
{
    if (!inRange(index)) {
        return QVariant();
    }

    if (role == LayerDataRole) {
        QVariant v;
        v.setValue(layers_[index.row()]);
        return v;
    }

    switch (index.column()) {
    case Col_Position:
        return dataForColPosition(index.row(), role);
    case Col_Type:
        return dataForColType(index.row(), role);
    case Col_UnitCount:
        return dataForColUnit(index.row(), role);
    case Col_ActFunc:
        return dataForColActFunc(index.row(), role);
    default:
        return QVariant();
    }
}

QVariant LayerDescModel::dataForColPosition(int row, int role) const
{
    switch (role) {
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::DisplayRole:
    {
        auto layer = layers_[row];
        if (layer.type == LayerDesc::Input) {
            return "I";
        } else if (layer.type == LayerDesc::Output) {
            return "O";
        } else {
            return "H";
        }
    }
    default:
        return QVariant();
    }
}

QVariant LayerDescModel::dataForColUnit(int row, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    {
        auto layer = layers_[row];
        if (layer.type == LayerDesc::Input) {
            return "-";
        } else if (layer.type == LayerDesc::Output) {
            return "-";
        } else {
            return layer.nUnit;
        }
    }
    case Qt::EditRole:
        return layers_[row].nUnit;
    default:
        return QVariant();
    }
}

QVariant LayerDescModel::dataForColType(int row, int role) const
{
    switch (role) {
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::DisplayRole:
    {
        auto layer = layers_[row];
        switch (layer.type)
        {
        case LayerDesc::Input:
            return "Input";
        case LayerDesc::FullyConnected:
            return "FullyConnected";
        case LayerDesc::Output:
            return "Output";
        default:
            return "Unknown";
        }
    }
    case Qt::EditRole:
        return layers_[row].type;
    default:
        return QVariant();
    }
}

QVariant LayerDescModel::dataForColActFunc(int row, int role) const
{
    switch (role) {
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::DisplayRole:
    {
        auto layer = layers_[row];
        switch (layer.activationFunc)
        {
        case LayerDesc::TANH:
            return "Tanh";
        case LayerDesc::LOGISTIC:
            return "Logistic";
        case LayerDesc::TANH_SCALED:
            return "Tanh (scaled)";
        case LayerDesc::RECTIFIER:
            return "Rectifier";
        case LayerDesc::LINEAR:
            return "Identity";
        default:
            return "Unknown";
        }
    }
    case Qt::EditRole:
        return layers_[row].activationFunc;
    default:
        return QVariant();
    }
}

QVariant LayerDescModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
    if (role != Qt::DisplayRole) { return QVariant(); }

    if (orientation != Qt::Horizontal) {
        return section + 1;
    }

    switch (section) {
    case Col_Position:
        return tr("Pos");
    case Col_Type:
        return tr("Type");
    case Col_UnitCount:
        return tr("Units");
    case Col_ActFunc:
        return tr("Act");
    default:
        return QVariant();
    }
}

int LayerDescModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) { return 0; }
    return layers_.size();
}

int LayerDescModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) { return 0; }
    return ColCount;
}

bool LayerDescModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!inRange(index)) {
        return false;
    }

    if (role == LayerDataRole) {
        layers_[index.row()] = value.value<LayerDesc>();
    }

    if (role != Qt::EditRole) {
        return false;
    }

    switch (index.column()) {
    case Col_Type:
        layers_[index.row()].type = LayerDesc::LayerType(value.toInt());
        break;
    case Col_UnitCount:
        layers_[index.row()].nUnit = value.toInt();
        break;
    case Col_ActFunc:
        layers_[index.row()].activationFunc = LayerDesc::ActivationFunction(value.toInt());
        break;
    default:
        return false;
    }

    return true;
}
bool LayerDescModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid()) {
        return false;
    }

    if (row == -1) {
        row = rowCount();
    }

    beginInsertRows(parent, row, row + count - 1);
    while (count--) {
        layers_.insert(row, LayerDesc());
    }
    endInsertRows();
    return true;
}

bool LayerDescModel::insertLayer(int position, const LayerDesc &layer)
{
    // treate position at -1 as at end
    position = position == -1 ? layers_.size() : position;
    // perform insert
    QModelIndex idx;
    beginInsertRows(idx, position, position);
    layers_.insert(position, layer);
    endInsertRows();
    return true;
}

bool LayerDescModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid()) {
        return false;
    }

    if (row + count > layers_.size()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    while (count--) {
        layers_.removeAt(row);
    }
    endRemoveRows();
    return false;
}

bool LayerDescModel::inRange(const QModelIndex &index) const
{
    return index.isValid()
        && index.row() >= 0 && index.row() < layers_.size()
        && index.column() >= 0 && index.column() < ColCount;
}

void LayerDescModel::clear()
{
    layers_.clear();
}

const QList<LayerDesc> &LayerDescModel::layers() const
{
    return layers_;
}
