/**
 * @file history_model.h
 * @brief 历史记录数据模型
 */

#ifndef CALC_UI_QT_MODELS_HISTORY_MODEL_H
#define CALC_UI_QT_MODELS_HISTORY_MODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QVector>
#include <QVariantMap>

namespace calc::ui::qt {

/**
 * @brief 历史记录数据模型
 */
class HistoryModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        ExpressionRole = Qt::UserRole + 1,
        ResultRole,
        TimestampRole,
        ModeRole,
        SuccessRole
    };

    explicit HistoryModel(QObject* parent = nullptr);
    ~HistoryModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Model operations
    Q_INVOKABLE void addItem(const QVariantMap& item);
    Q_INVOKABLE void addItems(const QVariantList& items);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE QVariantMap getItem(int index) const;

    // Persistence
    Q_INVOKABLE bool saveToFile(const QString& filename);
    Q_INVOKABLE bool loadFromFile(const QString& filename);

    // Filtering
    void setModeFilter(const QString& mode);
    QString getModeFilter() const;

private:
    QVector<QVariantMap> items_;
    QString modeFilter_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_MODELS_HISTORY_MODEL_H
