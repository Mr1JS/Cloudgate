#ifndef IO_HDF5IO_HPP
#define IO_HDF5IO_HPP

#include "../SharedArray.hpp"
#include "Hdf5Util.hpp"

#include <H5Tpublic.h>
#include <hdf5_hl.h>
#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5File.hpp>
#include <memory>
#include <tuple>
#include <type_traits>

namespace jumper
{

/**
 * @class Hdf5IO
 * @brief Manager Class for all Hdf5IO components located in hdf5 directory
 */
template <template <typename> typename... Features>
class BaseHdf5IO : public Features<BaseHdf5IO<Features...>>...
{
  protected:
    /**
     * @brief type trait to check if a given feature is loaded
     *
     * @tparam T feature to check for
     */
    template <typename T, typename Tuple>
    struct has_type;

    /**
     * @brief type trait to check if a given feature is loaded
     *
     * @tparam T feature to check for
     * @tparam U current element of loaded teatures to check for
     * @tparam Ts remaining features to check for
     */
    template <typename T, typename U, typename... Ts>
    struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>>
    {
    };

    template <typename T, typename... Ts>
    struct has_type<T, std::tuple<T, Ts...>> : std::true_type
    {
    };

    template <typename T>
    struct has_type<T, std::tuple<>> : std::false_type
    {
    };

  public:
    static constexpr std::size_t N = sizeof...(Features);
    using features                 = std::tuple<Features<BaseHdf5IO<Features...>>...>;

    /**
     * @brief type trait to check if a given feature is loaded
     *
     * @tparam F feature to check for
     */
    template <template <typename> typename F>
    struct has_feature
    {
        static constexpr bool value = has_type<F<BaseHdf5IO>, features>::type::value;
    };

    BaseHdf5IO() : m_compress(true), m_chunkSize(1e7), m_usePreviews(true) {}

    virtual ~BaseHdf5IO() {}

    /**
     * @brief opens a given file
     *
     * @param filename name of the file to open
     */
    void open(std::string filename);

    /**
     * @brief checks whether or not the current Hdf5IO has a given feature
     *
     * @tparam T type of the feature to check for
     *
     * @return true if the feature is loaded, false else
     */
    template <template <typename> typename F>
    bool has();

    /**
     * @brief casts this hdf5io to a given feature statically
     *
     * @tparam F feature to cast to
     *
     * @return the requested feature
     */
    template <template <typename> typename F>
    F<BaseHdf5IO>* scast();

    /**
     * @brief casts this hdf5io to a given feature dynamically
     *
     * @tparam F feature to cast to
     *
     * @return the requested feature
     */
    template <template <typename> typename F>
    F<BaseHdf5IO>* dcast();

    /**
     * @brief loads an array from hdf5
     *
     * @tparam T datatype if array elements
     * @param groupName name of hdf5 group to read from
     * @param datasetName name of the dataset holding the array
     * @param dim holds the size of the array after loading is done
     *
     * @return array holding the loaded data
     */
    template <typename T>
    shared_array<T>
    loadArray(std::string groupName, std::string datasetName, std::vector<size_t>& dim);

    /**
     * @brief loads an array from hdf5
     *
     * @tparam T datatype if array elements
     * @param groupName name of hdf5 group to read from
     * @param datasetName name of the dataset holding the array
     * @param dim holds the size of the array after loading is done
     *
     * @return array holding the loaded data
     */
    template <typename T>
    shared_array<T>
    loadArray(HighFive::Group& groupName, std::string datasetName, std::vector<size_t>& dim);

    /**
     * @brief saves an array to the hdf5 file
     *
     * @tparam T datatype of array elements
     * @param groupName name of group to write to
     * @param datasetName name of the dataset
     * @param data array to save
     */
    template <typename T>
    void save(std::string groupName, std::string datasetName, size_t size, shared_array<T> data);

    /**
     * @brief saves an array to the hdf5 file
     *
     * @tparam T datatype of array elements
     * @param groupName name of group to write to
     * @param datasetName name of the dataset
     * @param dim array size per dimension
     * @param data array to save
     */
    template <typename T>
    void save(std::string groupName,
              std::string datasetName,
              std::vector<size_t>& dimensions,
              shared_array<T> data);

    /**
     * @brief saves an array to the hdf5 file
     *
     * @tparam T datatype of array elements
     * @param groupName name of group to write to
     * @param datasetName name of the dataset
     * @param dim array size per dimension
     * @param chunkSize chunk size in hdf5 file (usually same as dim)
     * @param data array to save
     */
    template <typename T>
    void save(std::string groupName,
              std::string datasetName,
              std::vector<size_t>& dimensions,
              std::vector<hsize_t>& chunkSize,
              shared_array<T> data);

    /**
     * @brief saves an array to the hdf5 file
     *
     * @tparam T datatype of array elements
     * @param g group to write to
     * @param datasetName name of the dataset
     * @param dim array size per dimension
     * @param chunkSize chunk size in hdf5 file (usually same as dim)
     * @param data array to save
     */
    template <typename T>
    void save(HighFive::Group& g,
              std::string datasetName,
              std::vector<size_t>& dim,
              std::vector<hsize_t>& chunkSize,
              shared_array<T>& data);

    bool m_compress;
    size_t m_chunkSize;
    bool m_usePreviews;
    unsigned int m_previewReductionFactor;
    std::string m_filename;
    std::shared_ptr<HighFive::File> m_hdf5_file;
};

} // namespace jumper

#include "BaseHdf5IO.tcc"

#endif // LVR2_IO_GHDF5IO_HPP

