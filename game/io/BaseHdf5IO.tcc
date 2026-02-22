/**
 * @file BaseHdf5IO.tcc
 * @brief Template implementation for the BaseHdf5IO class
 */

namespace jumper
{

template <template <typename> typename... Features>
void BaseHdf5IO<Features...>::open(std::string filename)
{

    m_filename        = filename;
    this->m_hdf5_file = hdf5util::open(filename);

    if (!m_hdf5_file->isValid())
    {
        throw std::runtime_error("[BaseHdf5IO] Hdf5 file not valid!");
        return;
    }
}

template <template <typename> typename... Features>
template <template <typename> typename F>
bool BaseHdf5IO<Features...>::has()
{
    return BaseHdf5IO<Features...>::template has_feature<F>::value;
}

template <template <typename> typename... Features>
template <template <typename> typename F>
F<BaseHdf5IO<Features...>>* BaseHdf5IO<Features...>::scast()
{
    return static_cast<F<BaseHdf5IO<Features...>>*>(this);
}

template <template <typename> typename... Features>
template <template <typename> typename F>
F<BaseHdf5IO<Features...>>* BaseHdf5IO<Features...>::dcast()
{
    return dynamic_cast<F<BaseHdf5IO<Features...>>*>(this);
}

template <template <typename> typename... Features>
template <typename T>
shared_array<T> BaseHdf5IO<Features...>::loadArray(std::string groupName,
                                            std::string datasetName,
                                            std::vector<size_t>& dim)
{
    HighFive::Group g = hdf5util::getGroup(m_hdf5_file, groupName, false);

    return loadArray<T>(g, datasetName, dim);
}

template <template <typename> typename... Features>
template <typename T>
shared_array<T>
BaseHdf5IO<Features...>::loadArray(HighFive::Group& g, std::string datasetName, std::vector<size_t>& dim)
{
    shared_array<T> ret;

    if (m_hdf5_file && m_hdf5_file->isValid())
    {
        if (g.exist(datasetName))
        {
            HighFive::DataSet dataset = g.getDataSet(datasetName);
            dim                       = dataset.getSpace().getDimensions();

            size_t elementCount = 1;
            for (auto e : dim)
                elementCount *= e;

            if (elementCount)
            {
                ret = shared_array<T>(new T[elementCount]);

                dataset.read(ret.get());
            }
        }
    }
    else
    {
        throw std::runtime_error("[Hdf5 - ArrayIO]: Hdf5 file not open.");
    }

    return ret;
}

template <template <typename> typename... Features>
template <typename T>
void BaseHdf5IO<Features...>::save(std::string groupName,
                            std::string datasetName,
                            size_t size,
                            shared_array<T> data)
{
    std::vector<size_t> dim = {size, 1};
    std::vector<hsize_t> chunks{m_chunkSize, 1};
    HighFive::Group g = hdf5util::getGroup(m_hdf5_file, groupName);
    save(g, datasetName, dim, chunks, data);
}

template <template <typename> typename... Features>
template <typename T>
void BaseHdf5IO<Features...>::save(std::string groupName,
                            std::string datasetName,
                            std::vector<size_t>& dimensions,
                            shared_array<T> data)
{
    HighFive::Group g = hdf5util::getGroup(m_hdf5_file, groupName);

    // Compute chunk size vector, i.e., set the chunk size in
    // each dimension to default size. Add float array will
    // trim this values if chunkSize > dim.
    std::vector<hsize_t> chunks;
    for (auto i : dimensions)
    {
        chunks.push_back(i);
    }
    save(g, datasetName, dimensions, chunks, data);
}

template <template <typename> typename... Features>
template <typename T>
void BaseHdf5IO<Features...>::save(std::string groupName,
                            std::string datasetName,
                            std::vector<size_t>& dimensions,
                            std::vector<hsize_t>& chunkSize,
                            shared_array<T> data)
{
    HighFive::Group g = hdf5util::getGroup(m_hdf5_file, groupName);
    save(g, datasetName, dimensions, chunkSize, data);
}

template <template <typename> typename... Features>
template <typename T>
void BaseHdf5IO<Features...>::save(HighFive::Group& g,
                            std::string datasetName,
                            std::vector<size_t>& dim,
                            std::vector<hsize_t>& chunkSizes,
                            shared_array<T>& data)
{
    if (m_hdf5_file && m_hdf5_file->isValid())
    {

        HighFive::DataSpace dataSpace(dim);
        HighFive::DataSetCreateProps properties;

        if (m_chunkSize)
        {
            // We have to check explicitly if chunk size
            // is < dimensionality to avoid errors from
            // the HDF5 lib
            for (size_t i = 0; i < chunkSizes.size(); i++)
            {
                if (chunkSizes[i] > dim[i])
                {
                    chunkSizes[i] = dim[i];
                }
            }
            properties.add(HighFive::Chunking(chunkSizes));
        }
        if (m_compress)
        {
            // properties.add(HighFive::Shuffle());
            properties.add(HighFive::Deflate(9));
        }

        std::unique_ptr<HighFive::DataSet> dataset
            = hdf5util::createDataset<T>(g, datasetName, dataSpace, properties);

        const T* ptr = data.get();
        dataset->write(ptr);
        m_hdf5_file->flush();
    }
    else
    {
        throw std::runtime_error("[Hdf5 - ArrayIO]: Hdf5 file not open.");
    }
}

} // namespace jumper
