/**
 * @file mode_model.cpp
 * @brief 计算模式数据模型实现
 */

#include "calc/ui/qt/models/mode_model.h"

namespace calc::ui::qt {

ModeModel::ModeModel(QObject* parent)
    : QAbstractListModel(parent)
{
    // 初始化默认模式
    QVariantMap standardMode;
    standardMode["name"] = "standard";
    standardMode["displayName"] = "Standard";
    standardMode["description"] = "Basic arithmetic operations";
    standardMode["icon"] = "standard";

    QVariantMap scientificMode;
    scientificMode["name"] = "scientific";
    scientificMode["displayName"] = "Scientific";
    scientificMode["description"] = "Advanced mathematical functions";
    scientificMode["icon"] = "scientific";

    QVariantMap programmerMode;
    programmerMode["name"] = "programmer";
    programmerMode["displayName"] = "Programmer";
    programmerMode["description"] = "Base conversion and bitwise operations";
    programmerMode["icon"] = "programmer";

    modes_ = {standardMode, scientificMode, programmerMode};
}

ModeModel::~ModeModel() = default;

int ModeModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return modes_.size();
}

QVariant ModeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= modes_.size()) {
        return QVariant();
    }

    const QVariantMap& mode = modes_[index.row()];

    switch (role) {
        case Qt::DisplayRole:
        case DisplayNameRole:
            return mode["displayName"];
        case NameRole:
            return mode["name"];
        case DescriptionRole:
            return mode["description"];
        case IconRole:
            return mode["icon"];
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> ModeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DisplayNameRole] = "displayName";
    roles[DescriptionRole] = "description";
    roles[IconRole] = "icon";
    return roles;
}

void ModeModel::setModes(const QVector<QVariantMap>& modes)
{
    beginResetModel();
    modes_ = modes;
    endResetModel();
}

void ModeModel::addMode(const QVariantMap& mode)
{
    beginInsertRows(QModelIndex(), modes_.size(), modes_.size());
    modes_.append(mode);
    endInsertRows();
}

void ModeModel::clear()
{
    beginResetModel();
    modes_.clear();
    endResetModel();
}

int ModeModel::findIndex(const QString& name) const
{
    for (int i = 0; i < modes_.size(); ++i) {
        if (modes_[i]["name"].toString() == name) {
            return i;
        }
    }
    return -1;
}

QVariantMap ModeModel::getMode(const QString& name) const
{
    int index = findIndex(name);
    if (index >= 0) {
        return modes_[index];
    }
    return QVariantMap();
}

QVariantMap ModeModel::getModeAt(int index) const
{
    if (index >= 0 && index < modes_.size()) {
        return modes_[index];
    }
    return QVariantMap();
}

} // namespace calc::ui::qt
