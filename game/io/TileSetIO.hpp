#ifndef IO_TILESETIO_HPP
#define IO_TILESETIO_HPP

#include "../include/TileSetRepresentation.hpp"

namespace jumper
{

  namespace hdf5features
  {

    template <typename Derived>
    class TileSetIO
    {
    public:
      /**
       * @brief loads a tileset from hdf5
       *
       * hands over ownership of returned Tiles object!
       *
       * @tparam Tiles type of tileset (must support TileSetRepresentations constructor)
       * @param groupName name of group to load from
       * @param datasetName name of dataset that holds the tiles
       * @param tileW width of one tile
       * @param tileH height of one tile
       *
       * @return Tiles object that contains the tiles
       */
      template <typename Tiles>
      Tiles *load(std::string groupName, std::string datasetName, int tileW, int tileH);

      /**
       * @brief loads a tileset from hdf5
       *
       * hands over ownership of returned Tiles object!
       *
       * @tparam Tiles type of tileset (must support TileSetRepresentations constructor)
       * @param group group to load from
       * @param datasetName name of dataset that holds the tiles
       * @param tileW width of one tile
       * @param tileH height of one tile
       *
       * @return Tiles object that contains the tiles
       */
      template <typename Tiles>
      Tiles *load(HighFive::Group group, std::string datasetName, int tileW, int tileH);

      /**
       * @brief saves a TileSetRepresentation into hdf5
       *
       * @param groupName name of hdf5 group
       * @param datasetName name of hdf5 dataset
       * @param tiles tile to write
       */
      void save(std::string groupName, std::string datasetName, const TileSetRepresentation &tiles);

      /**
       * @brief saves a TileSetRepresentation into hdf5
       *
       * @param group hdf5 group
       * @param datasetName name of hdf5 dataset
       * @param tiles tile to write
       */
      void save(HighFive::Group group, std::string datasetName, const TileSetRepresentation &tiles);

      /**
       * @brief loads a raw array from hdf5
       *
       * hands over ownership of returned raw array!
       *
       * @param groupName name of hdf5 group
       * @param datasetName name of dataset that holds the data
       * @param dims vector for storing the size of the array
       *
       * @return raw array that holds the read data
       */
      int **loadRaw(std::string groupName, std::string datasetName, std::vector<size_t> &dims);

    private:
      Derived *m_fileAccess = static_cast<Derived *>(this);
    };

  } // namespace hdf5features

} // namespace jumper

#include "TileSetIO.tcc"

#endif
