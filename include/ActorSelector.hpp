/**
 * @file ActorSelector.hpp
 * @brief Defines the ActorSelector class for selecting actor sprites in the level editor
 */

#include <QObject>
#include <QVector>
#include <QPixmap>
#include <QVariant>

class ActorSelector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant currentActor READ currentActor NOTIFY currentActorChanged)
    Q_PROPERTY(QString currentActorName READ currentActorName NOTIFY currentActorChanged)

public:
    explicit ActorSelector(QObject *parent = nullptr);

    QString currentActor() const;
    QString currentActorName() const;

public slots:
    void next();
    void prev();
    void reset();
    void setStartActor();
    void updateAllActorFiles(const QString &newActor);

signals:
    void currentActorChanged();

private:
    void loadActors();

    QMap<QString, QString>  m_actors;
    int                     m_currentIndex;
    QList<QString>          m_actorNames;
};
