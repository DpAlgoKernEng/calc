/**
 * @file history_model.cpp
 * @brief 历史记录数据模型实现
 */

#include "calc/ui/qt/models/history_model.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace calc::ui::qt {

HistoryModel::HistoryModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

HistoryModel::~HistoryModel() = default;

int HistoryModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (!modeFilter_.isEmpty()) {
        int count = 0;
        for (const auto& item : items_) {
            if (item["mode"].toString() == modeFilter_) {
                count++;
            }
        }
        return count;
    }
    return static_cast<int>(items_.size());
}

QVariant HistoryModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= items_.size()) {
        return QVariant();
    }

    const QVariantMap& item = items_[index.row()];

    // 应用模式过滤
    if (!modeFilter_.isEmpty() && item["mode"].toString() != modeFilter_) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
        case ExpressionRole:
            return item["expression"];
        case ResultRole:
            return item["result"];
        case TimestampRole:
            return item["timestamp"];
        case ModeRole:
            return item["mode"];
        case SuccessRole:
            return item["success"].toBool();
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> HistoryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ExpressionRole] = "expression";
    roles[ResultRole] = "result";
    roles[TimestampRole] = "timestamp";
    roles[ModeRole] = "mode";
    roles[SuccessRole] = "success";
    return roles;
}

void HistoryModel::addItem(const QVariantMap& item)
{
    int size = static_cast<int>(items_.size());
    beginInsertRows(QModelIndex(), size, size);
    items_.append(item);
    endInsertRows();
}

void HistoryModel::addItems(const QVariantList& items)
{
    if (items.isEmpty()) {
        return;
    }

    int currentSize = static_cast<int>(items_.size());
    int newSize = static_cast<int>(items.size());
    beginInsertRows(QModelIndex(), currentSize, currentSize + newSize - 1);
    for (const QVariant& item : items) {
        items_.append(item.toMap());
    }
    endInsertRows();
}

void HistoryModel::clear()
{
    beginResetModel();
    items_.clear();
    endResetModel();
}

void HistoryModel::removeItem(int index)
{
    if (index < 0 || index >= items_.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    items_.removeAt(index);
    endRemoveRows();
}

QVariantMap HistoryModel::getItem(int index) const
{
    if (index < 0 || index >= items_.size()) {
        return QVariantMap();
    }
    return items_[index];
}

bool HistoryModel::saveToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray jsonArray;
    for (const auto& item : items_) {
        QJsonObject obj;
        obj["expression"] = item["expression"].toString();
        obj["result"] = item["result"].toString();
        obj["timestamp"] = item["timestamp"].toString();
        obj["mode"] = item["mode"].toString();
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
    return true;
}

bool HistoryModel::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) {
        return false;
    }

    beginResetModel();
    items_.clear();

    QJsonArray jsonArray = doc.array();
    for (const QJsonValue& value : jsonArray) {
        QJsonObject obj = value.toObject();
        QVariantMap item;
        item["expression"] = obj["expression"].toString();
        item["result"] = obj["result"].toString();
        item["timestamp"] = obj["timestamp"].toString();
        item["mode"] = obj["mode"].toString();
        items_.append(item);
    }

    endResetModel();
    return true;
}

void HistoryModel::setModeFilter(const QString& mode)
{
    beginResetModel();
    modeFilter_ = mode;
    endResetModel();
}

QString HistoryModel::getModeFilter() const
{
    return modeFilter_;
}

} // namespace calc::ui::qt
