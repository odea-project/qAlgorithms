#ifndef _QALGORITHMS_GLOBALS_INCLUDED
#define _QALGORITHMS_GLOBALS_INCLUDED

#include <array>

namespace qAlgorithms
{
    extern float PPM_PRECENTROIDED;
    extern float MZ_ABSOLUTE_PRECENTROIDED;

    extern std::array<float, 384> INV_ARRAY; // @todo make const
    static constexpr float T_VALUES[256] = {6.313751515, 2.91998558, 2.353363435, 2.131846781, 2.015048373, 1.94318028, 1.894578605, 1.859548038, 1.833112933, 1.812461123, 1.795884819, 1.782287556, 1.770933396, 1.761310136, 1.753050356, 1.745883676, 1.739606726, 1.734063607, 1.729132812, 1.724718243, 1.720742903, 1.717144374, 1.713871528, 1.71088208, 1.708140761, 1.70561792, 1.703288446, 1.701130934, 1.699127027, 1.697260894, 1.695518789, 1.693888742, 1.692360304, 1.690924251, 1.689572454, 1.688297711, 1.687093617, 1.685954458, 1.684875119, 1.683851011, 1.682878, 1.681952356, 1.681070702, 1.680229975, 1.679427392, 1.678660413, 1.677926721, 1.677224195, 1.676550892, 1.675905025, 1.67528495, 1.674689153, 1.674116236, 1.673564906, 1.673033965, 1.672522303, 1.672028888, 1.671552762, 1.671093032, 1.670648865, 1.670219484, 1.669804162, 1.669402222, 1.669013025, 1.668635976, 1.668270514, 1.667916114, 1.667572281, 1.667238549, 1.666914479, 1.666599658, 1.666293696, 1.665996224, 1.665706893, 1.665425373, 1.665151353, 1.664884537, 1.664624644, 1.664371409, 1.664124579, 1.663883913, 1.663649184, 1.663420175, 1.663196679, 1.6629785, 1.662765449, 1.662557349, 1.662354029, 1.662155326, 1.661961084, 1.661771155, 1.661585397, 1.661403674, 1.661225855, 1.661051817, 1.66088144, 1.66071461, 1.660551217, 1.660391156, 1.660234326, 1.66008063, 1.659929976, 1.659782273, 1.659637437, 1.659495383, 1.659356034, 1.659219312, 1.659085144, 1.658953458, 1.658824187, 1.658697265, 1.658572629, 1.658450216, 1.658329969, 1.65821183, 1.658095744, 1.657981659, 1.657869522, 1.657759285, 1.657650899, 1.657544319, 1.657439499, 1.657336397, 1.65723497, 1.657135178, 1.657036982, 1.656940344, 1.656845226, 1.656751594, 1.656659413, 1.656568649, 1.65647927, 1.656391244, 1.656304542, 1.656219133, 1.656134988, 1.65605208, 1.655970382, 1.655889868, 1.655810511, 1.655732287, 1.655655173, 1.655579143, 1.655504177, 1.655430251, 1.655357345, 1.655285437, 1.655214506, 1.655144534, 1.6550755, 1.655007387, 1.654940175, 1.654873847, 1.654808385, 1.654743774, 1.654679996, 1.654617035, 1.654554875, 1.654493503, 1.654432901, 1.654373057, 1.654313957, 1.654255585, 1.654197929, 1.654140976, 1.654084713, 1.654029128, 1.653974208, 1.653919942, 1.653866317, 1.653813324, 1.653760949, 1.653709184, 1.653658017, 1.653607437, 1.653557435, 1.653508002, 1.653459126, 1.6534108, 1.653363013, 1.653315758, 1.653269024, 1.653222803, 1.653177088, 1.653131869, 1.653087138, 1.653042889, 1.652999113, 1.652955802, 1.652912949, 1.652870547, 1.652828589, 1.652787068, 1.652745977, 1.65270531, 1.652665059, 1.652625219, 1.652585784, 1.652546746, 1.652508101, 1.652469842, 1.652431964, 1.65239446, 1.652357326, 1.652320556, 1.652284144, 1.652248086, 1.652212376, 1.652177009, 1.652141981, 1.652107286, 1.65207292, 1.652038878, 1.652005156, 1.651971748, 1.651938651, 1.651905861, 1.651873373, 1.651841182, 1.651809286, 1.651777679, 1.651746359, 1.65171532, 1.65168456, 1.651654074, 1.651623859, 1.651593912, 1.651564228, 1.651534805, 1.651505638, 1.651476725, 1.651448062, 1.651419647, 1.651391475, 1.651363544, 1.65133585, 1.651308391, 1.651281164, 1.651254165, 1.651227393, 1.651200843, 1.651174514, 1.651148402, 1.651122505, 1.65109682, 1.651071345, 1.651046077, 1.651021013, 1.650996152, 1.65097149, 1.650947025, 1.650922755, 1.650898678, 1.650874791, 1.650851092, 1.650827579};
    static constexpr float CHI_SQUARES[256] = {3.84146, 5.99146, 7.81473, 9.48773, 11.0705, 12.59159, 14.06714, 15.50731, 16.91898, 18.30704, 19.67514, 21.02607, 22.36203, 23.68479, 24.99579, 26.29623, 27.58711, 28.8693, 30.14353, 31.41043, 32.67057, 33.92444, 35.17246, 36.41503, 37.65248, 38.88514, 40.11327, 41.33714, 42.55697, 43.77297, 44.98534, 46.19426, 47.39988, 48.60237, 49.80185, 50.99846, 52.19232, 53.38354, 54.57223, 55.75848, 56.94239, 58.12404, 59.30351, 60.48089, 61.65623, 62.82962, 64.00111, 65.17077, 66.33865, 67.50481, 68.66929, 69.83216, 70.99345, 72.15322, 73.31149, 74.46832, 75.62375, 76.7778, 77.93052, 79.08194, 80.2321, 81.38102, 82.52873, 83.67526, 84.82065, 85.96491, 87.10807, 88.25016, 89.39121, 90.53123, 91.67024, 92.80827, 93.94534, 95.08147, 96.21667, 97.35097, 98.48438, 99.61693, 100.74862, 101.87947, 103.00951, 104.13874, 105.26718, 106.39484, 107.52174, 108.64789, 109.77331, 110.898, 112.02199, 113.14527, 114.26787, 115.38979, 116.51105, 117.63165, 118.75161, 119.87094, 120.98964, 122.10773, 123.22522, 124.34211, 125.45842, 126.57415, 127.68931, 128.80391, 129.91796, 131.03146, 132.14442, 133.25686, 134.36878, 135.48018, 136.59107, 137.70146, 138.81136, 139.92077, 141.0297, 142.13816, 143.24615, 144.35367, 145.46074, 146.56736, 147.67353, 148.77926, 149.88456, 150.98943, 152.09388, 153.1979, 154.30152, 155.40472, 156.50752, 157.60992, 158.71193, 159.81355, 160.91478, 162.01563, 163.1161, 164.2162, 165.31593, 166.4153, 167.5143, 168.61295, 169.71125, 170.8092, 171.9068, 173.00406, 174.10098, 175.19757, 176.29382, 177.38975, 178.48535, 179.58063, 180.6756, 181.77025, 182.86458, 183.95861, 185.05233, 186.14575, 187.23887, 188.33169, 189.42422, 190.51646, 191.6084, 192.70007, 193.79144, 194.88254, 195.97336, 197.06391, 198.15418, 199.24418, 200.33391, 201.42337, 202.51258, 203.60152, 204.6902, 205.77863, 206.8668, 207.95472, 209.04239, 210.12981, 211.21698, 212.30391, 213.3906, 214.47705, 215.56326, 216.64924, 217.73498, 218.82049, 219.90577, 220.99082, 222.07565, 223.16025, 224.24462, 225.32878, 226.41272, 227.49644, 228.57994, 229.66323, 230.7463, 231.82917, 232.91182, 233.99427, 235.07651, 236.15855, 237.24038, 238.32201, 239.40344, 240.48467, 241.56571, 242.64654, 243.72719, 244.80764, 245.8879, 246.96797, 248.04784, 249.12754, 250.20704, 251.28636, 252.3655, 253.44445, 254.52322, 255.60182, 256.68023, 257.75847, 258.83653, 259.91441, 260.99212, 262.06966, 263.14702, 264.22422, 265.30124, 266.3781, 267.45479, 268.53131, 269.60767, 270.68387, 271.7599, 272.83577, 273.91148, 274.98703, 276.06242, 277.13765, 278.21272, 279.28764, 280.36241, 281.43702, 282.51148, 283.58578, 284.65994, 285.73394, 286.80779, 287.8815, 288.95506, 290.02847, 291.10174, 292.17486, 293.24784, 294.32067};

    static const double SQRTPI_2 = 0.886226925452758013649;
    extern bool PRINT_PPM_WARNING;
    static const double OS_CRIT_A = 0.1443340625173891; // this value is determined empirically (see https://github.com/GeRe87/OS_critVal)
    static const double OS_CRIT_B = 3.2412322699344687; // this value is determined empirically (see https://github.com/GeRe87/OS_critVal)

    // f-values precalculated using the qf() function in R with alpha = 0.05
    // given are the values for df1 = n - 1, df2 = n - 4.
    static constexpr double F_VALUES[256] = {0.1297244, 0.1728269, 0.2102138, 0.2427001, 0.2711865, 0.2964058, 0.318932, 0.3392141, 0.3576058, 0.3743882, 0.389788, 0.4039893, 0.4171439, 0.4293777, 0.4407962, 0.4514887, 0.4615311, 0.4709888, 0.479918, 0.4883676, 0.4963803, 0.5039936, 0.5112406, 0.5181507, 0.52475, 0.5310617, 0.5371068, 0.5429041, 0.5484706, 0.5538215, 0.5589709, 0.5639316, 0.568715, 0.5733317, 0.5777916, 0.5821035, 0.5862757, 0.5903158, 0.5942308, 0.5980271, 0.6017109, 0.6052878, 0.6087628, 0.612141, 0.6154269, 0.6186246, 0.6217382, 0.6247714, 0.6277277, 0.6306104, 0.6334225, 0.6361669, 0.6388464, 0.6414636, 0.644021, 0.6465207, 0.648965, 0.651356, 0.6536957, 0.6559858, 0.6582282, 0.6604245, 0.6625763, 0.6646851, 0.6667524, 0.6687796, 0.670768, 0.6727188, 0.6746331, 0.6765123, 0.6783573, 0.6801692, 0.681949, 0.6836977, 0.6854162, 0.6871053, 0.6887659, 0.6903988, 0.6920048, 0.6935846, 0.695139, 0.6966687, 0.6981742, 0.6996563, 0.7011156, 0.7025526, 0.7039679, 0.7053621, 0.7067358, 0.7080893, 0.7094233, 0.7107382, 0.7120344, 0.7133124, 0.7145726, 0.7158155, 0.7170415, 0.7182509, 0.7194441, 0.7206215, 0.7217834, 0.7229302, 0.7240622, 0.7251798, 0.7262832, 0.7273727, 0.7284487, 0.7295114, 0.730561, 0.731598, 0.7326225, 0.7336347, 0.734635, 0.7356236, 0.7366006, 0.7375664, 0.7385211, 0.739465, 0.7403982, 0.741321, 0.7422335, 0.743136, 0.7440287, 0.7449117, 0.7457851, 0.7466492, 0.7475042, 0.7483502, 0.7491873, 0.7500158, 0.7508357, 0.7516472, 0.7524505, 0.7532457, 0.7540329, 0.7548123, 0.755584, 0.7563482, 0.7571049, 0.7578542, 0.7585964, 0.7593315, 0.7600596, 0.7607809, 0.7614954, 0.7622033, 0.7629046, 0.7635995, 0.764288, 0.7649704, 0.7656465, 0.7663166, 0.7669808, 0.7676391, 0.7682916, 0.7689384, 0.7695795, 0.7702152, 0.7708454, 0.7714702, 0.7720897, 0.772704, 0.7733132, 0.7739173, 0.7745164, 0.7751105, 0.7756998, 0.7762843, 0.776864, 0.7774391, 0.7780096, 0.7785755, 0.779137, 0.779694, 0.7802467, 0.7807951, 0.7813392, 0.7818791, 0.7824149, 0.7829466, 0.7834743, 0.783998, 0.7845177, 0.7850336, 0.7855457, 0.7860539, 0.7865585, 0.7870593, 0.7875565, 0.7880501, 0.7885401, 0.7890266, 0.7895097, 0.7899893, 0.7904655, 0.7909384, 0.7914079, 0.7918742, 0.7923373, 0.7927972, 0.7932539, 0.7937075, 0.794158, 0.7946055, 0.7950499, 0.7954914, 0.7959299, 0.7963655, 0.7967983, 0.7972281, 0.7976552, 0.7980795, 0.798501, 0.7989198, 0.7993359, 0.7997493, 0.8001601, 0.8005683, 0.8009739, 0.801377, 0.8017775, 0.8021756, 0.8025711, 0.8029642, 0.8033549, 0.8037432, 0.8041291, 0.8045127, 0.8048939, 0.8052728, 0.8056495, 0.8060239, 0.8063961, 0.806766, 0.8071338, 0.8074994, 0.8078629, 0.8082243, 0.8085835, 0.8089407, 0.8092958, 0.8096488, 0.8099999, 0.8103489, 0.810696, 0.8110411, 0.8113842, 0.8117255, 0.8120648, 0.8124022, 0.8127378, 0.8130715, 0.8134033, 0.8137334, 0.8140616, 0.814388};
}

#endif