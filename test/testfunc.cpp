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

struct OrderIndices_int                 // ßßß implement as template
        {
            const std::vector<int> &target;
            OrderIndices_int(const std::vector<int> &target) : target(target) {}
            bool operator()(int a, int b) const { return target[a] < target[b]; }
        };

int main()
{
    std::vector<double> v{0, 4, 6, 9};
    std::vector<double> i{2,1,4,3};
    print("Initially, v = ", v);
    std::sort(v.begin(), v.end(), OrderIndices(i));
    print("Modified v = ", v);

    std::sort(v.begin(), v.end(),
           [&](int A, int B) -> bool {
                return i[A] < i[B];
            });
print("Modified v2 = ", v);
}
