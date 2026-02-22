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
    // for more readable code
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        FileRole,
        GoalTypeRole,
        GoalValueRole
    };

    explicit LevelSelector(QObject *parent = nullptr);
    // QAbstractListModel functions to transfer infos between qml and cpp
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void reload();

private:
    // struct to coordinate the data better
    struct Level 
    {
        QString name;
        QString file;
        int     goalType = 0;  
        int     goalValue = 0; 
    };

    QVector<Level> m_levels;
    void scanFolder();
};
