#ifndef IO_TEXTUREIO_HPP
#define IO_TEXTUREIO_HPP

#include <SDL.h>

namespace jumper
{

namespace hdf5features
{

template <typename Derived>
class TextureIO
{
  public:
    /**
     * @brief loads an sdl surface from hdf5
     *
     * hands over ownership of returned sdl_surface!
     *
     * @param groupName name of group of image
     * @param datasetName name of hdf5 dataset that holds the image
     *
     * @return Sdl surface holding the loaded image
     */
    SDL_Surface* load(std::string groupName, std::string datasetName);

    /**
     * @brief loads an sdl surface from hdf5
     *
     * hands over ownership of returned sdl_surface!
     *
     * @param group group of image
     * @param datasetName name of hdf5 dataset that holds the image
     *
     * @return Sdl surface holding the loaded image
     */
    SDL_Surface* load(HighFive::Group group, std::string datasetName);

    /**
     * @brief saves an sdl surface to hdf5 file
     *
     * @param groupName nae of group in hdf5 to save to
     * @param datasetName name of dataset
     * @param texture sdl surface to write
     */
    void save(std::string groupName, std::string datasetName, SDL_Surface* texture);

    /**
     * @brief saves an sdl surface to hdf5 file
     *
     * @param group group in hdf5 to save to
     * @param datasetName name of dataset
     * @param texture sdl surface to write
     */
    void save(HighFive::Group group, std::string datasetName, SDL_Surface* texture);

    /**
     * @brief loads an image from file and saves it into hdf5
     *
     * @param groupName name of group to save to
     * @param datasetName name of datasez to save to
     * @param imageFileName name of image file
     */
    void saveImage(std::string groupName, std::string datasetName, std::string imageFileName);

    /**
     * @brief loads an image with keying color from file and saves it into hdf5
     *
     * @param groupName name of group to save to
     * @param datasetName name of datasez to save to
     * @param imageFileName name of image file
     * @param keyR red part of keying color
     * @param keyG green part of keying color
     * @param keyB blue part of keying color
     */
    void saveImage(std::string groupName,
                   std::string datasetName,
                   std::string imageFileName,
                   unsigned char keyR,
                   unsigned char keyG,
                   unsigned char keyB);

  private:
    Derived* m_fileAccess       = static_cast<Derived*>(this);
};

} // namespace hdf5features

} // namespace jumper

#include "TextureIO.tcc"

#endif
