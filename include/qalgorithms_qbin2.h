#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iterator>
#include <thread> 

// Goal: all functions modify individual features, which are combined to a bin. The bin contains all features and the functions necessary to summarise which features are present

namespace q
{
    // Feature Struct (contains all user-specified variables found in raw data and an index)
    struct Feature
    {
        int idx;
        double mz;
        double mzError;
        double RT;
        int scanNo;
    };
    
    // FeatureList
    
    // Bin Struct ()


}