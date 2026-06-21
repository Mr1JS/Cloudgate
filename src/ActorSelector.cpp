/**
 * @file ActorSelector.cpp
 * @brief Implementation of the ActorSelector class for managing actor sprite selection
 *        in the level editor, loading actor images and providing selection interface
 */

#include "include/ActorSelector.hpp"
#include <QDirIterator>
#include <QFileInfo>
#include <QCoreApplication>
#include <QImage>
#include "game/include/Util.hpp"

ActorSelector::ActorSelector(QObject *parent)
    : QObject(parent), m_currentIndex(0)
{
    loadActors();
}

QString ActorSelector::currentActor() const
{
    if (m_actors.isEmpty() || m_currentIndex < 0 || m_currentIndex >= m_actorNames.size())
    {
        return QString();
    }
    QString key = m_actorNames[m_currentIndex];
    return m_actors.value(key);
}

QString ActorSelector::currentActorName() const
{
    if (m_actorNames.isEmpty() || m_currentIndex < 0 || m_currentIndex >= m_actorNames.size())
    {
        return QString();
    }
    return m_actorNames[m_currentIndex];
}

// right button clicked
void ActorSelector::next()
{
    if (!m_actors.isEmpty())
    {
        // start at 0 again if too often clicked
        m_currentIndex = (m_currentIndex + 1) % m_actorNames.size();
        emit currentActorChanged();
    }
}
// left button clicked
void ActorSelector::prev()
{
    if (!m_actors.isEmpty())
    {
        // start at max again if too often clicked
        m_currentIndex = (m_currentIndex - 1 + m_actors.size()) % m_actorNames.size();
        emit currentActorChanged();
    }
}

void ActorSelector::reset()
{
    m_currentIndex = 0;
    emit currentActorChanged();
}

// get all Actors listed in assets.qrc
void ActorSelector::loadActors()
{
    // iterate through actors
    QDirIterator it(":/resources/images/actors",
                    QStringList() << "*.png",
                    QDir::Files);

    while (it.hasNext())
    {
        QString actor = it.next();
        QFileInfo info(actor);
        QString actorName = info.baseName();
        if (!actor.isNull())
        {
            m_actors.insert(actorName, ("qrc" + actor));
            m_actorNames.append(actorName);
        }
    }
}
// check and set last selected character to show when clicking Character button
void ActorSelector::setStartActor()
{
    // get path of master_level file
    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.cdUp();
    std::string absoluteLevelPath = appDir.absoluteFilePath("res/level_master.xml").toStdString();
    std::string actorName = jumper::getLevelActor(absoluteLevelPath);

    // get index
    int index = m_actorNames.indexOf(QString::fromStdString(actorName));
    if (index != -1)
    {
            m_currentIndex = index;
            emit currentActorChanged();
    }

}
// go through all level.xml files and change character
void ActorSelector::updateAllActorFiles(const QString &newActor)
{
    qDebug() << "Character Selected: " << newActor;
    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.cdUp();
    QDir levelDir = appDir.absoluteFilePath("res");

    QStringList xmlFiles = levelDir.entryList(QStringList() << "*.xml", QDir::Files);

    for (const QString &file : xmlFiles) 
    {
        QString filePath = levelDir.absoluteFilePath(file);
        jumper::updateActor(filePath.toStdString(), newActor.toStdString());
    }
}
