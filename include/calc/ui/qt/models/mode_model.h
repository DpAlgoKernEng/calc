/**
 * @file mode_model.h
 * @brief 计算模式数据模型
 */

#ifndef CALC_UI_QT_MODELS_MODE_MODEL_H
#define CALC_UI_QT_MODELS_MODE_MODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QVariantMap>

namespace calc::ui::qt {

/**
 * @brief 计算模式数据模型
 */
struct ModeInfo {
    QString name;
    QString displayName;
    QString description;
    QString icon;

    ModeInfo() = default;
    ModeInfo(const QString& n, const QString& dn, const QString& d, const QString& i)
        : name(n), displayName(dn), description(d), icon(i) {}
};

class ModeModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        DisplayNameRole,
        DescriptionRole,
        IconRole
    };

    explicit ModeModel(QObject* parent = nullptr);
    ~ModeModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Model operations
    Q_INVOKABLE void setModes(const QVector<QVariantMap>& modes);
    Q_INVOKABLE void addMode(const QVariantMap& mode);
    Q_INVOKABLE void clear();

    Q_INVOKABLE int findIndex(const QString& name) const;
    Q_INVOKABLE QVariantMap getMode(const QString& name) const;
    Q_INVOKABLE QVariantMap getModeAt(int index) const;

private:
    QVector<QVariantMap> modes_;
};

} // namespace calc::ui::qt

#endif // CALC_UI_QT_MODELS_MODE_MODEL_H
