#include <vector>
#include <cmath>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

main()
{
    std::ifstream file("./test.csv");
    std::vector<std::string> data_names;
    std::vector<std::vector<double>> data;
    

    static bool initialised;

    std::size_t line_number = 0;
    std::istringstream stream{};
    
    

    for (std::string line{}; std::getline(file, line); stream.clear())
    {
        stream.str(line);
        std::string number{};
        std::size_t position = 0;

        if (!initialised)
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

        while (std::getline(stream, number, ','))
            {
                std::cout << "number " << number << " -- ";
                std::cout << "position " << position << " -- ";
                data[position].push_back(stod(number));
                ++position;
                std::cout << "\n";
            }
        
        //std::cout << "Line " << line_number++ << " contains " << nums << " col length " << columns << "\n";
    }
    std::cout << data[2][2];
    //     while (std::getline(file, line1)) {
    //         std::cout << line1;
    //       std::stringstream ss(line1);
    //       std::string cell;
    //       std::vector<double> row;

    //       while (std::getline(ss, cell, ',')) {
    //           row.push_back(std::stod(cell));
    //       }

    //       data.push_back(row);
    //   }

    //   std::cout << data[0][1];
}

//
