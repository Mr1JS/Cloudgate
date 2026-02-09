/*
 *  Util.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "game/include/Util.hpp"

#include <iostream>
#include <fstream>
#include <SDL_image.h>
#include <sstream>
#include <regex>
#include <QDebug>
// NEED for reading data from XML
#include <QXmlStreamReader>
#include <QFile>

namespace jumper
{

SDL_Texture* LoadTexture(
        SDL_Renderer* renderer, std::string texFileName,
        unsigned char key_r, unsigned char key_g, unsigned char key_b)
{
    // The loaded texture
    SDL_Texture* newTexture = nullptr;

    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(texFileName.c_str());
    if( loadedSurface == nullptr )
    {
        std::cout << "Unable to load image! SDL_image Error: " <<  IMG_GetError() << std::endl;
        return nullptr;
    }
    else
    {
        // Set keying color for some unknown reason SDL_MapRGB returns the index with the first element being index 1 ¯\_(ツ)_/¯
        // Said index is for the loadedSurface->format->palette->colors array
        Uint32 key_color_index = SDL_MapRGB( loadedSurface->format, key_r, key_g, key_b);
        if (key_color_index > 0) key_color_index--;
        if(SDL_SetColorKey( loadedSurface, SDL_TRUE, key_color_index))
        {
            std::cout << "SDL could not set color key: " << SDL_GetError() << std::endl;
        }
        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if( newTexture == nullptr )
        {
            std::cout <<  "Unable to create texture from! SDL Error: " << texFileName <<  SDL_GetError() << std::endl;
            return nullptr;
        }

        // Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }
    return newTexture;
}

std::string GetExtensionFromFileName(std::string filename)
{
    // Get path from given .lvl file
    size_t position = filename.find_last_of(".");
    if(position != std::string::npos)
    {
        return filename.substr(position + 1);
    }
    return "";
}

std::string GetPathFromFileName(std::string filename)
{
    size_t position = filename.find_last_of("/\\");
    return filename.substr(0, position + 1);
}


/**
 * @brief Loads tile definitions from XML including collision shape.
 * Optional attribute: shape="full"|"half_bottom"|"half_left"|"half_right"|"half_top"|"diag_tl_br"|"diag_tr_bl"
 */
std::map<int, TileInfo> ParseXMLData(const std::string& xmlPath)
{
    std::map<int, TileInfo> tileData;

    QFile file(QString::fromStdString(xmlPath));
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qWarning() << "Could not open XML file:" << xmlPath.c_str();
        return tileData;
    }

    QXmlStreamReader xml(&file);
    while (!xml.atEnd())
    {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == QString("tile"))
        {
            QXmlStreamAttributes attr = xml.attributes();
            int id = attr.value("id").toInt();
            TileInfo info;
            info.name  = attr.value("name").toString().toStdString();
            info.type  = attr.value("type").toString().toStdString();
            QString sh = attr.value("shape").toString();
            info.shape = sh.isEmpty() ? "full" : sh.toStdString();
            tileData[id] = info;
        }
    }

    if (xml.hasError())
    {
        qWarning() << "XML parsing error:" << xml.errorString();
    }

    file.close();
    return tileData;
}
// get last saved actor to show when loading character page or when saving new level
std::string getLevelActor(const std::string& path)
{
    // read xml of level_master (all level files contain the current actor)
    QFile file(QString::fromStdString(path));
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qWarning() << "Could not open XML file:" << path.c_str();
        return "";
    }

    QXmlStreamReader xml(&file);
    std::string startActor;

    // search for name of actor
    while (!xml.atEnd() && !xml.hasError()) 
    {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == QString("actor")) 
        {
            QXmlStreamAttributes attrs = xml.attributes();
            if (attrs.hasAttribute("texture")) 
            {
                startActor = attrs.value("texture").toString().toStdString();
                break; 
            }
        }
    }

    if (xml.hasError()) 
    {
        qWarning() << "XML parsing error:" << xml.errorString();
    }
    // return actor
    file.close();
    return startActor;
}

// easier and probably more efficient for only rewriting one value than using QXmlStreamReader and QXmlStreamWriter open at the same time
void updateActor(const std::string &filePath, const std::string &newActor)
{
    // open file
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) 
    {
        std::cerr << "Cannot open file: " << filePath << std::endl;
        return;
    }

    // get content of xml file
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();

    // find <actor texture="…">
    std::regex rx(R"(<actor\s+texture="[^"]*")");
    content = std::regex_replace(content, rx, "<actor texture=\"" + newActor + "\"");

    // rewrite file
    std::ofstream outFile(filePath, std::ios::trunc);
    if (!outFile.is_open()) 
    {
        std::cerr << "Cannot write file: " << filePath << std::endl;
        return;
    }

    outFile << content;
    outFile.close();
}



} // namespace jumper
