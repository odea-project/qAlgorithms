// #include <../include/qalgorithms_qbin.h>

#include <vector>

namespace q
{
    namespace qPattern
    {
        struct FeaturePeak
        {
            double mz;
            double RT;
            double mzErr;
            double RTerr;
        };

        enum MassDifference
        {
            a13C,
            // _16O,
            sH_aNa,
            sH_aK,
            sH2O
        };

        std::vector<double> massDiffs =
            {
                1.0033 // a13C
        };

        // struct AdductPath
        // {
        //     // implements the general form [M + n(Kation) - (n-1)H]+
        //     MassDifference cation;
        //     int cationCount;
        //     int cationCharge;
        // };

        struct PeakConnector
        {
            // connection type - one of adducts, isotopolouges or neutral loss
            // the only identifier of this property is the absolute difference
            // in mz. "type" is constructed by following a network and pushing
            // back every step
            MassDifference step;
            std::vector<MassDifference> type;
            // connected points
            int *origin;
            int *end;

            // 0 for origin peak, then counts up. This is set after network construction.
            int chainPosition;
            // this ID is unique and used to refer to the connector when walking a tree
            int orderID;

            // total shift the assignment introduces into the system
            double shiftMZ;
            double shiftRT;

            double errorcodeMatch; // overlap between codes in percent

            // quality score shift
            bool DQSCincrease;
            bool DQSBincrease;
            bool DQSPincrease;
            bool reversible; // does this matter?
        };

        struct Peaklist
        {
            std::vector<std::vector<FeaturePeak>> featurelist;
            std::vector<std::vector<PeakConnector>> connectorlist;
        };

    }
}