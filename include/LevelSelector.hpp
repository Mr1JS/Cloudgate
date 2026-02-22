/**
 * @file LevelSelector.hpp
 * @brief Defines the LevelSelector class for selecting levels to play or edit
 */

#include <QAbstractListModel>
#include <QVector>
#include <QString>

class LevelSelector : public QAbstractListModel
{
    Q_OBJECT
public:

    /// enum with name, file and win condition data (for more readable code)
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        FileRole,
        GoalTypeRole,
        GoalValueRole
    };

    /// QObject based Constructor
    explicit LevelSelector(QObject *parent = nullptr);

    /// @return rowCount - QAbstractListModel function to transfer rowCount between qml and cpp
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /// @return data - QAbstractListModel function to transfer index and role between qml and cpp
    QVariant data(const QModelIndex &index, int role) const override;

    /// @return roleNames - QAbstractListModel function to transfer Roles enum between qml and cpp
    QHash<int, QByteArray> roleNames() const override;

    /// Reload the levels
    Q_INVOKABLE void reload();

private:

    /// Find all levels
    void scanFolder();

    /// Struct to coordinate the level data better
    struct Level 
    {
        QString name; /// Name of level
        QString file; /// File path of level
        int     goalType = 0;   /// Type of win condition
        int     goalValue = 0; /// Value of win condition
    };

    QVector<Level> m_levels; /// List of all levels
};
