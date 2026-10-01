#include "../src/qalgorithms_logging.cpp" // NOLINT
#include "common_test_utils.hpp"
#include "qalgorithms_logging.h"
#include <cstddef>

namespace qAlgorithms
{
    const char errorState[] = "qpeaks: 1921\neJxllXlQE1ccx18SDo8ixSICtQ5OISAgHkgWE8zbfQGSBXU8UEGcKnhhAQe0tEoUIgqKV+UQ60EBDzxQOXTAEe0LQqxcSgEBBQEtKCgKYgUMwnY2ZhwkO7Pz2z/efub7Pt99s2AC+HwZaOdE7XQ58aOXdf8Zul14WXqd9qM3nvxEFy88R+fuM6aDLVbQsDuPnrc/iza8bUMfv3ZRlrFPJVs+46i0/KMKvfqoQmPUKmSrViEPtQqtV6vQHrUKZahVSKlWoWa1Cg2pVWjKoAoJB1XIb1CFGqpDqEdZAdR7joAynulPjc0OopZXBlDxh3yomN5V1G37ddS1XYFUweMVVNm4BVR4wiIqwc+Zavdzq0ZOfLKVYZird4swuBnaSDfWYhDWmM9UlmAu4Gp3BIA+ACAmio+6e9LQ3pe7pSXOIYK5odUuviGGYpOGveJfUyYKh5loyAGA4Wjf6TrM/Sb1si0pN2p3mePZCsHABuM/iO04feosee2LOswDY76wDQEAL6rrkVNvAlruME2WyTkhKi7qIroK22BjbQAs3DdBWPkpGvJG8PcfAmdPXLQllQzDHNusxAo68ZxpSxsGp61pYdJlnfyOq+8gaW8aWt1gILsyqWJeyUk5kXzmW3Jtas38mHoHYd6o/CvnOhPuEnsyrNeybLF7JwQ+Jt7HTl2BykVZIn/XKszTpAZAT5s/NdrKXf4+AeXWrqN5ihpXu4p7hDf/JFwzHA69F3iKhpiv8ydnD84W7uKTvpIdRk4TnmBgtVTh61uGwQQf5x6f45AHeF/ys3zJzDzk25eGlJn50ounY0R5ps+Ipo9GpNe8LTBq4CdR7ig/RcU9glX/8skj31ma9ft1wFa9vMYuDxUEEspx6aYHWF+bn3XDNpE8uRNF9ieh59dsZYLDca4ziCxCr/AsrDm0HqY9yRP2MdFQbwSfu31qoA3kk20MwxRcSoes7nHSB1gBAGiU/KPj3z7OFrXw01FL8F5pjskBgZ7ypQA6Goqpvl2wzdxc5/vRv7vBYdl/dmQPwzAmXkqoCDCPm+1Titlpk3Yfj/ZzKWyh5N3rs2h1eYzs1PH7gjp7mghK2S92mrQTSl+5CZlR/tM9pscH/mVHdjAM00JVQeCdaO3XoMTsDK8ow1wtn6PN/8ZSJjG1yUROw9tlqcb5gni7Zheu0fr5a29shlZ7oIY/Mr91sT3H+jyfbGAYpvvheUw25Yeeb6nHiqb80IiDNTp+LCqlyEyWg1ZmZkjFF8oF2/K8ieb6xZBfKIfTFljq+Dl8vbbj/SQHzfk1jiyFrPeeN9WYxXZYVOjwQaS9e0FsEUqMQHTGeJLI6FEQlYZFYtf4QNgZu0wIgOIrfmxb3Om4eFuNHyfDKk2/t3aXa/pd2Vqvw7e5W4racREiY3kyh79zXWxaxC7ppTlux3OioScgdPjZim1yK7kjuViyw+idSTsEBhaTk3vyMbinb7HJoxzzNCfrs3+2X062qYfr60Lk6v6915uta1yXGFkS7hVnxEFz5HB94FoNf2S/98QBSft22JFNDMMEm+VjcMyaPrq0FAM7OtEu7I5Ov1MYf8mH7iJUE+ItexeUTXQfmUq0c8rEBm47obgzUsf/UJ8NcZPrqPH/e2MWBEnWdFhaLWZnyA9NmDvifLF3yvBC9/QPBUj8KIneaOItENzKEvwcaC7ubYuAKMJWx09UuI/A7pmThr/q6WPIBp1+oBGz8+DbWsz93KpmF+zThaFoD05/ITKLyPaKki90CX3g7+L/bPb81MHroqTgWTr8X8zuKs8lzSKrGIZ5iPKwggNAbUk1ZrWH6d/GXGCkWTdey1eEb/W8MnAVibZs9a5bU+o6ONBMOF8dhuueVrndKNmkw//T5okeT+xM9pj1hX9SvsXKaeri5vJSDG5JJXV3UqAeMNOsY/+bYwEAIsU02lOdhuIU9ouecxNEAwVPiQtLKiAX/QY7Xju7sXz9Efz/ARH0oNM=";
    const size_t obs_length = 14;
    const float obs_intensities[] = {2680397.25F, 1335012.5F, 87157.7891F, 890011.188F, 2069324.75F, 1345976.38F, 587899.062F, 876950.688F, 1465831.88F, 1405191.12F, 678848.438F, 345460.719F, 404541.031F, 73818.6641f}; // NOLINT
    const float obs_x[] = {98.98787690F, 98.98811340F, 98.98834230F, 98.9885788F, 98.9888077F, 98.9890442F, 98.9892807F, 98.9895096F, 98.9897461F, 98.989975F, 98.9902115F, 98.990448F, 98.9906769F, 98.9909134F};

    static void test_logger_recovery()
    {
        QPeaks_log_mapping testRecover = read_log_qpeaks(errorState);
        assert(testRecover.length == obs_length, "length parsed incorrectly\n", NULL);

        float *intensities = testRecover.get_intensities_ptr();
        float *intensities_log = testRecover.get_intensities_log_ptr();
        for (size_t i = 0; i < obs_length; i++)
        {
            assert(intensities[i] == obs_intensities[i], "Incorrect intensity recovered\n", NULL);
            assert(intensities_log[i] == log(obs_intensities[i]), "Incorrect log intensity recovered\n", NULL);
        }

        float *x_axis = testRecover.get_x_axis_ptr();
        for (size_t i = 0; i < obs_length; i++)
            assert(x_axis[i] == obs_x[i], "x axis not recovered\n", NULL);

        assert(testRecover.get_df_ptr() == nullptr, "incorrect presence of df array\n", NULL);
    }

} // namespace qAlgorithms

using namespace qAlgorithms;

int main(void)
{
    test_logger_recovery();
    return 0;
}