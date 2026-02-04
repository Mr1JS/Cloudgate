#include "include/LevelSelector.hpp"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QCoreApplication>
#include <QVector>
#include <QTextStream>
#include <QXmlStreamReader>

LevelSelector::LevelSelector(QObject *parent)
    : QAbstractListModel(parent)
{
    scanFolder();
}
// search for all xml files in res
void LevelSelector::scanFolder()
{
    // res path
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir = dir.filePath("res");

    // only scan xml
    QStringList filters;
    filters << "*.xml";

    // go through files
    QFileInfoList files = dir.entryInfoList(
        filters,
        QDir::Files | QDir::NoSymLinks,
        QDir::Name
    );

    beginResetModel();
    m_levels.clear();
    
    for (const QFileInfo &info : files) 
    {
        Level lvl;
        lvl.file = info.absoluteFilePath();
        lvl.name = info.baseName();

        if (lvl.name == "level_master") // master file should not appear
        {
            continue;
        }
        // filter goal infos from xml
        QFile f(lvl.file);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) 
        {
            QXmlStreamReader xml(&f);

            while (!xml.atEnd()) 
            {
                xml.readNext();

                if (xml.isStartElement()) 
                {
                    QString elem = xml.name().toString();

                    if (elem == "goal") 
                    {
                        while (!(xml.isEndElement() && xml.name() == "goal")) 
                        {
                            xml.readNext();

                            if (xml.isStartElement()) 
                            {
                                QString name = xml.name().toString();
                                if (name == "type")
                                {
                                    lvl.goalType = xml.readElementText().toInt();
                                }
                                if (name == "value")
                                {
                                    lvl.goalValue = xml.readElementText().toInt();
                                }
                            }
                        }
                    }
                }
            }
        }

        m_levels.append(lvl);
    }

    endResetModel();
}

void LevelSelector::reload()
{
    scanFolder();
}

// get row count to see how many levels need to be painted
int LevelSelector::rowCount(const QModelIndex &) const 
{
    return m_levels.size();
}

// easier access in qml
QVariant LevelSelector::data(const QModelIndex &index, int role) const 
{
    if (!index.isValid())
    {
        return {};
    }

    const Level &lvl = m_levels[index.row()];
    switch (role) {
        case NameRole: return lvl.name;
        case FileRole: return lvl.file;
        case GoalTypeRole: return lvl.goalType;
        case GoalValueRole: return lvl.goalValue;
        default: return {};
    }
}
// easier access in qml
QHash<int, QByteArray> LevelSelector::roleNames() const 
{
    return 
    {
        { NameRole, "name" },
        { FileRole, "file" },
        { GoalTypeRole, "goalType" },
        { GoalValueRole, "goalValue" }
    };
}
