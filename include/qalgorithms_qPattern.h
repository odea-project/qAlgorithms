#include <../include/qalgorithms_qbin.h>

namespace q
{
    namespace qPattern
    {
        enum MassDifference
        {
            a13C,
            // _16O,
            sH_aNa,
            sH_aK,
            sH2O
        };

        // struct AdductPath
        // {
        //     // implements the general form [M + n(Kation) - (n-1)H]+
        //     MassDifference cation;
        //     int cationCount;
        //     int cationCharge;
        // };

        class PeakConnector
        {
        public:
            PeakConnector(int *originPoint, int *endPoint);

            // connected points
            int *origin;
            int *end;
            bool reversible;

            // connection type - one of adducts, isotopolouges or neutral loss
            // the only identifier of this property is the absolute difference
            // in mz
            MassDifference type;

            // total shift the assignment introduces into the system
            double shiftMZ;
            double shiftRT;

            // quality score shift
            bool DQSCincrease;
            bool DQSBincrease;
            double errorcodeMatch; // overlap between scores in percent
        };

    }
}