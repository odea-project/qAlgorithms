#ifndef STREAMCRAFT_UTILS_HPP
#define STREAMCRAFT_UTILS_HPP

#include <string>
#include <vector>

namespace StreamCraft
{
    enum MS_SPECTRA_MODE
    {
        CENTROID,
        PROFILE
    };

    std::vector<double> decode_little_endian(const std::string &str, const int precision);

    std::string decompress_zlib(const std::string &compressed_string);

    std::string decode_base64(const std::string &encoded_string);
};

#endif // STREAMCRAFT_UTILS_HPP