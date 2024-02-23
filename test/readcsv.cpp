#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

main()
{
    std::ifstream file("./test.csv"); // test.csv must inclide a header line specifying the contents of each column
    std::vector<std::string> data_names;
    std::vector<std::vector<double>> data;
    static bool initialised;

    std::istringstream stream{};

    for (std::string line{}; std::getline(file, line); stream.clear())
    {
        stream.str(line);
        std::string number{};
        std::size_t position = 0;

        if (!initialised) // only execute this part once - uses same naming as numbers to remove header
        {
            initialised = true;
            while (std::getline(stream, number, ','))
            {
                data_names.push_back(number);
                std::cout << "Column Name " << number << " -- ";
            }
            data.resize(data_names.size());
            std::cout << "\n";
        }

        while (std::getline(stream, number, ',')) // every subvector contains one column
        {
            std::cout << "number " << number << " -- ";
            std::cout << "position " << position << " -- ";
            data[position].push_back(stod(number));
            ++position;
            std::cout << "\n";
        }

    }
    std::cout << data[2][2];
}

//
