// qalgorithms_qbin.cpp

#include "../include/qalgorithms_qbin.h"



namespace q {
// Functions
    // mean Distances to other elements in vector
const std::vector<double> fastMeanDistances(const std::vector<double>& x){
    const int n = static_cast<int>(x.size()); // always convert to int std::vector<double>
    // generate a vector vec_i of lenght n^2 with elements of 1:n repeating n times
    std::vector<int> vec_i;
    vec_i.reserve(n * n);
    for (int i = 1; i < n+1; i++)
    { 
        std::vector<int> vec_temp(n,i);
        vec_i.insert(vec_i.end(), vec_temp.begin(), vec_temp.end());
    }
    // generate a vector vec_j of lenght n^2 with the series 1:n repeated n times
    std::vector<int> vec_j;
    vec_j.reserve(n * n);
    for (int i = 1; i < n + 1; i++) // ßßß loop necessary?
    {
        std::vector<int> vec_temp(n);
        std::iota(vec_temp.begin(), vec_temp.end(), 1);
        vec_j.insert(vec_j.end(), vec_temp.begin(), vec_temp.end());
    }

    //vec_i.push_back

    for (size_t i = 0; i < vec_i.size(); ++i)
        //std::cout << vec_i[i] << " ";
        std::cout << vec_j[i] << " ";
    
    //std::cout << x.size() << std::endl;     
    return x;
    }

const std::vector<double> slowMeanDistances(const std::vector<double> &x){
    const int n = static_cast<int>(x.size());
    
    }

int main()
{
    std::vector<double> x {1,2,3};

    std::vector<double> y = q::fastMeanDistances(x);
    int c = 0;
    for (int i = 0; i < 10; i++) {  
       if (i == 5)
       {
        c = i;
       }
    }
    //std::cout << c;
    return 0;
}
