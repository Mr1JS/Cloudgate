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
 * @brief Loads tile names from an XML file
 *
 * Parses an XML file containing tile definitions and extracts
 * tile IDs with their corresponding names.
 *
 * @param xmlPath Path to the XML file
 * @return Map of tile IDs to tile names
 */
std::map<int, std::string> ParseXMLData(const std::string& xmlPath);

} // namespace jumper


#endif // TEXTUREFACTORY_H
