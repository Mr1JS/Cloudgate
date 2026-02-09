/*
 *  Util.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef UTILS_H
#define UTILS_H

#include <SDL.h>
#include <map> // Needed for saving data from XML
#include <string>

#include "TileArray.hpp"

namespace jumper
{

/**
 * @brief Load an SDL texture from a file
 * @param renderer The SDL renderer to create the texture with
 * @param filename Path to the texture file
 * @param key_r Transparency color, red channel
 * @param key_g Transparency color, green channel
 * @param key_b Transparency color, blue channel
 * @return The loaded texture, or NULL when an error occurs
 */
SDL_Texture* LoadTexture(
        SDL_Renderer* renderer,
        std::string filename,
        unsigned char key_r = 255,
        unsigned char key_g = 255,
        unsigned char key_b = 255);

/**
 * @brief Get the extension of a file path
 * @param filename File name or path
 * @return The file extension of \ref filename
 *
 * Examples:
 *
 * "dir/file.jpg" becomes "jpg".
 * "dir/file-without-extension" becomes "" (the empty string).
 */
std::string GetExtensionFromFileName(std::string filename);

/**
 * @brief Removes the last components from a path
 * @param filename Path to process (usually to a file)
 * @return The given path, without the last component (usually a file)
 *
 * Example:
 *
 * "../test/bla.xml" becomes "../test/".
 */
std::string GetPathFromFileName(std::string filename);


/**
 * @brief Tile-Infos aus RulesTiles.xml (name, type, Kollisionsform)
 * shape: "full" | "half_bottom" | "half_left" | "half_right" | "half_top"
 *        | "diag_tl_br" | "diag_tr_bl" (Diagonalen für Rampen/Hügel)
 */
struct TileInfo
{
    std::string name;
    std::string type;
    std::string shape = "full";
};

/**
 * @brief Lädt Tile-Definitionen aus einer XML-Datei inkl. Kollisionsform.
 * @param xmlPath Pfad zur RulesTiles.xml
 * @return Map Tile-ID -> TileInfo (name, type, shape)
 */
std::map<int, TileInfo> ParseXMLData(const std::string& xmlPath);

/**
 * @brief get current actor
 * @param path path to level_master
 * @return name of current actor
 */
std::string getLevelActor(const std::string& path);

/**
 * @brief update actor in all xmls
 * @param filePath path to directory of all levels
 * @param newActor name of new actor
 */
void updateActor(const std::string &filePath, const std::string &newActor);
} // namespace jumper


#endif // TEXTUREFACTORY_H
