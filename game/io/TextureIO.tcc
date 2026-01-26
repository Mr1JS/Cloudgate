#include <SDL_image.h>

namespace jumper
{

namespace hdf5features
{

template <typename Derived>
SDL_Surface* TextureIO<Derived>::load(std::string groupName, std::string datasetName)
{
    return load(hdf5util::getGroup(m_fileAccess->m_hdf5_file, groupName, false), datasetName);
}

template <typename Derived>
SDL_Surface* TextureIO<Derived>::load(HighFive::Group group, std::string datasetName)
{
    std::vector<size_t> dims;
    shared_array<unsigned char> tmpPixels
        = m_fileAccess->template loadArray<unsigned char>(group, datasetName, dims);

    unsigned char* pixels = new unsigned char[dims[0] * dims[1] * dims[2]];
    std::memcpy(pixels, tmpPixels.get(), dims[0] * dims[1] * dims[2]);

    return SDL_CreateRGBSurfaceFrom(pixels,
                                    dims[1],
                                    dims[0],
                                    8 * dims[2],
                                    dims[1] * dims[2],
                                    0x00ff0000,
                                    0x0000ff00,
                                    0x000000ff,
                                    0xff000000);
}

template <typename Derived>
void TextureIO<Derived>::save(std::string groupName, std::string datasetName, SDL_Surface* texture)
{
    return save(
        hdf5util::getGroup(m_fileAccess->m_hdf5_file, groupName, true), datasetName, texture);
}

template <typename Derived>
void TextureIO<Derived>::save(HighFive::Group group, std::string datasetName, SDL_Surface* texture)
{
    // convert surface into rgb format
    texture = SDL_ConvertSurfaceFormat(texture, SDL_PIXELFORMAT_RGBA8888, 0);

    // create shared array from sirface pixels
    shared_array<unsigned char> data(new unsigned char[texture->w * texture->h * 4]);
    for (int i = 0; i < texture->w * texture->h * 4; i++)
    {
        data[i] = static_cast<unsigned char*>(texture->pixels)[i];
    }
    SDL_FreeSurface(texture);

    // save array
    std::vector<size_t> dims
        = std::vector<size_t>{static_cast<size_t>(texture->h), static_cast<size_t>(texture->w), 4};
    std::vector<hsize_t> chunks = std::vector<hsize_t>{dims[0], dims[1], dims[2]};
    m_fileAccess->save(group, datasetName, dims, chunks, data);
}

template <typename Derived>
void TextureIO<Derived>::saveImage(std::string groupName,
                                   std::string datasetName,
                                   std::string imageFileName)
{
    SDL_Surface* loadedSurface = IMG_Load(imageFileName.c_str());
    if (loadedSurface != nullptr)
    {
        save(groupName, datasetName, loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }
}

template <typename Derived>
void TextureIO<Derived>::saveImage(std::string groupName,
                                   std::string datasetName,
                                   std::string imageFileName,
                                   unsigned char keyR,
                                   unsigned char keyG,
                                   unsigned char keyB)
{
    SDL_Surface* loadedSurface = IMG_Load(imageFileName.c_str());
    if (loadedSurface != nullptr)
    {
        SDL_SetColorKey(
            loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, keyR, keyG, keyB));

        save(groupName, datasetName, loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }
}

} // namespace hdf5features

} // namespace jumper
