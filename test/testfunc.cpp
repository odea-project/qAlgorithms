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

struct OrderIndices_int // ßßß implement as template
{
    const std::vector<int> &target;
    OrderIndices_int(const std::vector<int> &target) : target(target) {}
    bool operator()(int a, int b) const { return target[a] < target[b]; }
};

struct Employee
{
    int age;
    std::string name; // Does not participate in comparisons
};

// bool operator<(const Employee &lhs, const Employee &rhs)
// {
//     return lhs.age < rhs.age;
// }

int main()
{

    if (0 < 1 < 2)
    {
        std::cout << "works";
    }

    std::vector<Employee> v{{108, "Zaphod"}, {32, "Arthur"}, {108, "Ford"}};

    std::sort(v.begin(), v.end(), [](const auto &lhs, const auto &rhs)
              { return lhs.age < rhs.age; });

    // std::sort(v.begin(), v.end());

    for (const Employee &e : v)
        std::cout << e.age << ", " << e.name << '\n';
}

// int main()
// {
//     std::vector<double> v{0, 4, 6, 9};
//     std::vector<double> i{2,1,4,3};
//     print("Initially, v = ", v);
//     std::sort(v.begin(), v.end(), OrderIndices(i));
//     print("Modified v = ", v);

//     std::sort(v.begin(), v.end(),
//            [&](int A, int B) -> bool {
//                 return i[A] < i[B];
//             });
// print("Modified v2 = ", v);
// }
