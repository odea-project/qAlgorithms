#include "StreamCraft_utils.hpp"
#include <vector>
#include <string>
#include <cstring>
#include <zlib.h>
#include <cassert>

/*!
 * Decodes from a little endian binary string to a vector of doubles according a precision integer.
 */
std::vector<double> StreamCraft::decode_little_endian(const std::string &str, const int precision)
{
    assert(precision == 4 || precision == 8);
    std::vector<unsigned char> bytes(str.begin(), str.end());

    int bytes_size = (bytes.size() / precision);

    std::vector<double> result(bytes_size);

    for (int i = 0; i < bytes_size; ++i)
    {

        if (precision == 8)
        {
            result[i] = reinterpret_cast<double &>(bytes[i * precision]);
        }
        else
        {
            float floatValue;
            std::memcpy(&floatValue, &bytes[i * precision], sizeof(float));
            result[i] = static_cast<double>(floatValue);
        }
    }
    return result;
};

/*!
 * Decompresses a string using the zlib library (https://zlib.net/).
 */
std::string StreamCraft::decompress_zlib(const std::string &compressed_string)
{
    z_stream zs;

    memset(&zs, 0, sizeof(zs));

    inflateInit(&zs);

    zs.next_in = (Bytef *)compressed_string.data();

    zs.avail_in = compressed_string.size();

    char outbuffer[32768]; // @todo why this size?

    std::string outstring;

    int ret = Z_OK;
    while (ret == Z_OK)
    {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);

        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out)
        {
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }
    }
    inflateEnd(&zs);

    return outstring;
};

/*!
 * Decodes a Base64 string into a string with binary data.
 */
std::string StreamCraft::decode_base64(const std::string &encoded_string) // @todo performance critical. Replace with highly optimised library
{
    std::string decoded_string;

    decoded_string.reserve((encoded_string.size() * 3) / 4);

    int val = 0;
    int valb = -8;
    for (char c : encoded_string)
    {
        if (c == '=')
        {
            valb -= 6;
            continue;
        }
        if (c >= 'A' && c <= 'Z')
        {
            c -= 'A';
        }
        else if (c >= 'a' && c <= 'z')
        {
            c -= 'a' - 26;
        }
        else if (c >= '0' && c <= '9')
        {
            c -= '0' - 52;
        }
        else if (c == '+')
        {
            c = 62;
        }
        else if (c == '/')
        {
            c = 63;
        }
        else
        {
            continue;
        }
        val = (val << 6) + c;
        valb += 6;
        if (valb >= 0)
        {
            decoded_string.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return decoded_string;
};