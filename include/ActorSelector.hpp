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

    /// File path of the currently selected actor (brdige to qml)
    Q_PROPERTY(QVariant currentActor READ currentActor NOTIFY currentActorChanged)

    /// Name of the currently selected actor (bridge to qml)
    Q_PROPERTY(QString currentActorName READ currentActorName NOTIFY currentActorChanged)

public:

    /// QObject based Constructor
    explicit ActorSelector(QObject *parent = nullptr);

    /// @return current actor's path
    QString currentActor() const;

    /// @return current actor's name
    QString currentActorName() const;

public slots:

    /// Switch to next actor
    void next();

    /// Go back to last actor
    void prev();

    /// Start at first actor again
    void reset();

    /// Define the actor to start with
    void setStartActor();

    /// Set the selected actor in all level files
    void updateAllActorFiles(const QString &newActor);

signals:

    /// Emit when new actor is selected
    void currentActorChanged();

private:

    /// Get all available actors from res/assets.qrc
    void loadActors();

    QMap<QString, QString>  m_actors; /// All actors and their path
    int                     m_currentIndex; /// Index of currently selected actor
    QList<QString>          m_actorNames; /// Index based names of actors (bridge between m_currentIndex and m_actors)
};
