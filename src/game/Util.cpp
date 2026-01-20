/*
 *  Util.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "Util.hpp"

#include <iostream>
#include <fstream>
#include <SDL_image.h>

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

} // namespace jumper
