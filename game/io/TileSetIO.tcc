/**
 * @file TileSetIO.tcc
 * @brief Template implementation for the TileSetIO class
 */


namespace jumper
{

namespace hdf5features
{

template <typename Derived>
template <typename Tiles>
Tiles*
TileSetIO<Derived>::load(std::string groupName, std::string datasetName, int tileW, int tileH)
{
    return load<Tiles>(
        hdf5util::getGroup(m_fileAccess->m_hdf5_file, groupName, false), datasetName, tileW, tileH);
}

template <typename Derived>
template <typename Tiles>
Tiles*
TileSetIO<Derived>::load(HighFive::Group group, std::string datasetName, int tileW, int tileH)
{
    std::vector<size_t> dims;
    shared_array<int> data = m_fileAccess->template loadArray<int>(group, datasetName, dims);

    Tiles* result = new Tiles(0, 0, dims[1], dims[0], tileW, tileH);

    for (int y = 0; y < result->height(); y++)
    {
        for (int x = 0; x < result->width(); x++)
        {
            result->insert(x, y, data[y * result->width() + x]);
        }
    }

    return result;
}

template <typename Derived>
int**
TileSetIO<Derived>::loadRaw(std::string group, std::string datasetName, std::vector<size_t>& dims)
{
    shared_array<int> data = m_fileAccess->template loadArray<int>(group, datasetName, dims);

    int** result = new int* [dims[0]];

    for (size_t y = 0; y < dims[0]; y++)
    {
        result[y] = new int[dims[1]];

        for (size_t x = 0; x < dims[1]; x++)
        {
            result[y][x] = data[y * dims[1] + x];
        }
    }

    return result;
}

template <typename Derived>
void TileSetIO<Derived>::save(std::string groupName,
                              std::string datasetName,
                              const TileSetRepresentation& tiles)
{
    save(hdf5util::getGroup(m_fileAccess->m_hdf5_file, groupName, true), datasetName, tiles);
}

template <typename Derived>
void TileSetIO<Derived>::save(HighFive::Group group,
                              std::string datasetName,
                              const TileSetRepresentation& tiles)
{
    shared_array<int> data(new int[tiles.width() * tiles.height()]);

    for (int x = 0; x < tiles.width(); x++)
    {
        for (int y = 0; y < tiles.height(); y++)
        {
            data[y * tiles.width() + x] = tiles.get(x, y);
        }
    }

    std::vector<size_t> dims = std::vector<size_t>{(size_t)tiles.height(), (size_t)tiles.width()};
    // std::vector<size_t> dims    = std::vector<size_t>{tiles.width(), tiles.height()};
    std::vector<hsize_t> chunks = std::vector<hsize_t>{(size_t)dims[0], (size_t)dims[1]};
    m_fileAccess->save(group, datasetName, dims, chunks, data);
}

} // namespace hdf5features

} // namespace jumper
