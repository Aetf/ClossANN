#ifndef LAYERDESCMODEL_H
#define LAYERDESCMODEL_H

#include <QAbstractTableModel>
#include "models/learnparam.h"

class LayerDescModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Roles {
        LayerDataRole = Qt::UserRole + 1,
        UnusedRole
    };

    enum Columns {
        Col_Position = 0,
        Col_Type,
        Col_UnitCount,
        Col_ActFunc,
        ColCount,
    };

    explicit LayerDescModel(QObject *parent = 0);
    virtual ~LayerDescModel();

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    bool insertLayer(int position, const LayerDesc &layer);

    const QList<LayerDesc> &layers() const;

public slots:
    void clear();

protected:
    bool inRange(const QModelIndex &index) const;
    QVariant dataForColPosition(int row, int role) const;
    QVariant dataForColType(int row, int role) const;
    QVariant dataForColActFunc(int row, int role) const;

signals:

private:
    QList<LayerDesc> layers_;
};

#endif // LAYERDESCMODEL_H
