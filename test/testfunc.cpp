#include "../include/qalgorithms_qBin.h"

auto print = [](auto comment, auto const &sequence)
{
    std::cout << comment;
    for (const auto &n : sequence)
        std::cout << n << ' ';
    std::cout << '\n';
};

struct OrderIndices                
        {
            const std::vector<double> &target;
            OrderIndices(const std::vector<double> &target) : target(target) {}
            bool operator()(int a, int b) const { return target[a] < target[b]; }
        };

std::vector<double> makeNOS(int dataspace, std::vector<std::vector<double>> activeDim)
{
    std::vector<int> orderOfImportance = {0, 1};
    std::vector<double> activeNos;
    //
    std::vector<int> index(activeDim[0].size()); // .data
    std::iota(index.begin(), index.end(), 1);
    // sort index by size of the active dataspace
    std::sort(index.begin(), index.end(), OrderIndices(activeDim[dataspace])); // .data

    // if (dataspace == orderOfImportance[0])
    // {
    //     mainIndices = index;
    // }

    for (size_t i = 0; i+1 < index.size(); i++) // +1 since difference vector is one short
    {
        double diff = activeDim[dataspace][index[i + 1]] - activeDim[dataspace][index[i]];
        activeNos.push_back(diff);
    }

    activeNos.push_back(-1); // NOS vector has same length as data
    return activeNos;
}

int main()
{
    std::vector v{0, 4, 6, 9};
    std::vector<double> i{0,1,2,3};
    print("Initially, v = ", v);
    std::adjacent_difference(v.begin(), v.end(), v.begin());
    print("Modified v = ", v);

    std::vector<std::vector<double>> testdim{i,i};
    std::vector<double> vtest = makeNOS(0, testdim);
    print("diff: ", vtest);
}
