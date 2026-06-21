#ifndef ENDLESSLEVELGENERATOR_HPP
#define ENDLESSLEVELGENERATOR_HPP

#include <QObject>
#include <QString>

class EndlessLevelGenerator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastLevelType READ lastLevelType NOTIFY lastLevelTypeChanged)
public:
    explicit EndlessLevelGenerator(QObject* parent = nullptr);

    /// Erzeugt ein prozedurales Level und gibt den Pfad zur endless.xml zurück
    /// \a levelNumber aktuelle Level-Nummer (1, 2, ...) – Tür wird pro Level 5 Tiles höher
    Q_INVOKABLE QString generateLevel(int levelNumber = 1);

    QString lastLevelType() const { return m_lastLevelType; }

signals:
    void lastLevelTypeChanged();

private:
    void setLastLevelType(const QString& type);

    QString m_resPath;
    QString m_lastLevelType;
};

#endif // ENDLESSLEVELGENERATOR_HPP
