#ifndef STREAMCRAFT_UTILS_HPP
#define STREAMCRAFT_UTILS_HPP

#include <iostream>
#include <string>
#include <vector>

namespace StreamCraft
{
    enum MS_SPECTRA_MODE
    {
        CENTROID,
        PROFILE
    };

    struct MS_SPECTRA_HEADERS
    {
        std::vector<int> index;
        std::vector<std::string> id;
        std::vector<int> scan;
        std::vector<int> array_length;
        std::vector<int> level;
        std::vector<std::string> mode;
        std::vector<std::string> polarity;
        std::vector<double> lowmz;
        std::vector<double> highmz;
        std::vector<double> bpmz;
        std::vector<double> bpint;
        std::vector<double> tic;
        std::vector<std::string> title;
        std::vector<double> rt;
        std::vector<double> drift;
        std::vector<std::string> filter_string;
        std::vector<int> config;
        std::vector<double> injection_ion_time;
        std::vector<int> precursor_scan;
        std::vector<double> window_mz;
        std::vector<double> window_mzlow;
        std::vector<double> window_mzhigh;
        std::vector<double> precursor_mz;
        std::vector<double> precursor_intensity;
        std::vector<int> precursor_charge;
        std::vector<std::string> activation_type;
        std::vector<double> activation_ce;

        void resize_all(int n)
        {
            index.resize(n);
            id.resize(n);
            scan.resize(n);
            array_length.resize(n);
            level.resize(n);
            mode.resize(n);
            polarity.resize(n);
            lowmz.resize(n);
            highmz.resize(n);
            bpmz.resize(n);
            bpint.resize(n);
            tic.resize(n);
            title.resize(n);
            rt.resize(n);
            drift.resize(n);
            filter_string.resize(n);
            config.resize(n);
            injection_ion_time.resize(n);
            precursor_scan.resize(n);
            window_mz.resize(n);
            window_mzlow.resize(n);
            window_mzhigh.resize(n);
            precursor_mz.resize(n);
            precursor_intensity.resize(n);
            precursor_charge.resize(n);
            activation_type.resize(n);
            activation_ce.resize(n);
        }
    };

    struct MS_CHROMATOGRAMS_HEADERS
    {
        std::vector<int> index;
        std::vector<std::string> id;
        std::vector<int> array_length;
        std::vector<std::string> polarity;
        std::vector<double> precursor_mz;
        std::vector<std::string> activation_type;
        std::vector<double> activation_ce;
        std::vector<double> product_mz;

        void resize_all(int n)
        {
            index.resize(n);
            id.resize(n);
            array_length.resize(n);
            polarity.resize(n);
            precursor_mz.resize(n);
            activation_type.resize(n);
            activation_ce.resize(n);
            product_mz.resize(n);
        };
    };

    std::string encode_little_endian(const std::vector<double> &input, const int &precision);

    std::vector<double> decode_little_endian(const std::string &str, const int &precision);

    std::string encode_big_endian(const std::vector<double> &input, const int &precision);

    std::vector<double> decode_big_endian(const std::string &str, const int &precision);

    std::string compress_zlib(const std::string &str);

    std::string decompress_zlib(const std::string &compressed_string);

    std::string encode_base64(const std::string &str);

    std::string decode_base64(const std::string &encoded_string);

    void test_encoding_decoding_little_endian(const std::vector<double> &input, const int &precision);

    void test_encoding_decoding_big_endian(const std::vector<double> &input, const int &precision);
};

#endif // STREAMCRAFT_UTILS_HPP

// #if defined(STREAMCRAFT_HEADER_ONLY) && !defined(STREAMCRAFT_UTILS_SOURCE)
// #	define STREAMCRAFT_UTILS_SOURCE "StreamCraft_utils.cpp"
// #	include STREAMCRAFT_UTILS_SOURCE
// #endif
