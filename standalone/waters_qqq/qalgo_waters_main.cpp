// the goal of this program is to read and process data generated in triplequad experiments automatically.
// it functions by reading in a list of inputs and extracting relevant masses in known RT regions supplied via a config file
// while conversion to mzml is necessary as of now, ideally the data could be read from waters raw directly
// refer to https://github.com/evanyeyeye/rainbow for direct reading

#include <vector>
#include <filesystem>
#include <iostream>
#include <errno.h>

#include "../../include/qalgorithms_read_file.h"
#include "../../include/qalgorithms_input_output.h"

namespace qalgo_waters
{
    // ### data structures ### @todo consider making a unified input thing if sciex files should also be processed

    // The relevant RT region is supplied via user input.
    struct Region_RT
    {
        float startRT;
        float endRT;
    };

    struct Region_mz
    {
        float min_mz;
        float max_mz;
    };

    struct SpectraRegion
    {
        std::vector<size_t> relevantSpectra; // these are individual scans
        Region_mz mzLimits;
        Region_RT rtLimits;
    };

    struct SearchEntry
    {
        std::vector<double> fragment_mz;
        std::string name = "";
        double RT_start = -1;
        double RT_end = -1;
        double tolerance_mz = -1;
        // bool ppm; // only consider that later, if at all
        // bool dalton;
    };

    std::vector<SearchEntry> getSearchConfig(const std::filesystem::path path)
    {
        // validate file format @todo how are the inputs structured?
        /*
            SEARCHFILE_START

            # (comment)
            ENTRY_START
            NAME = Target
            RT = 123:456
            tolerance_mz = 10 [ppm] # also possible: [Da] for absolute mz value? @todo
            fragments # output is always ordered by m/z
            = 57.1234
            = 600.3002
            = 230.1
            = 402.11000453
            fragments_end
            ENTRY_END

            ENTRY_START
            ...

            SEARCHFILE_END
        */

        std::vector<std::string> tasklist_parsed;
        if (!std::filesystem::exists(path))
        {
            std::cerr << "Error: the file " << path << " does not exist.\n";
            return {};
        }
        auto taskfile = fopen(path.string().c_str(), "rt"); // rt = read text
        if (taskfile == NULL)
        {
            std::cerr << "Error: readTaskList_txt() can not open " << path << ": " << strerror(errno) << "\n";
            return {};
        }
        char buf[260]; // 260 is the maximum path length in windows by default, so this should be fine @note
        while (fgets(buf, sizeof buf, taskfile) != NULL)
        {
            tasklist_parsed.push_back(buf);
        }
        assert(feof(taskfile));
        fclose(taskfile);

        if (tasklist_parsed.front() != "SEARCHFILE_START")
        {
            std::cerr << "Error: unexpected format: The file does not start with SEARCHFILE_START\n";
        }
        if (tasklist_parsed.back() != "SEARCHFILE_END")
        {
            std::cerr << "Error: unexpected format: The file does not start with SEARCHFILE_END\n";
        }

        // all lines with unexpected input are ignored
        std::vector<SearchEntry> res;
        bool openBlock = false;
        bool mzOpen = false;
        for (size_t i = 1; i < tasklist_parsed.size() - 1; i++)
        {
            auto line = tasklist_parsed[i];
            if (line == "#")
            {
                continue;
            }
            if (line == "ENTRY_START")
            {
                if (openBlock)
                {
                    std::cerr << "Error in line " << i << ": you forgot to close an Entry with \"ENTRY_END\"\n";
                    exit(1); // @todo show error log
                }
                openBlock = true;
                res.push_back({});
                continue;
            }
            if (line == "ENTRY_END")
            {
                if (!openBlock)
                {
                    std::cerr << "Error in line " << i << ": you forgot to start an Entry with \"ENTRY_END\"\n";
                    exit(1); // @todo show error log
                }
                openBlock = false;
                continue;
            }
            if (!openBlock)
            {
                continue; // @todo is this a good idea? ask users
            }
            if (line.size() > 4 && line.substr(0, 4) == "NAME")
            {
                assert(res.back().name.empty());
                // cull spaces from the name
                size_t namestart = line.find('=');
                size_t stringstart = line.find_first_not_of(' ', namestart);
                res.back().name = line.substr(stringstart);
                continue;
            }
            if (line.size() > 2 && line.substr(0, 2) == "RT")
            {
                assert(res.back().RT_start == -1 && res.back().RT_end == -1);
                // the two retention time limits are separated with a colon :
                size_t regionStart = line.find('=');
                size_t colonPos = line.find(':');
                double rtStart = std::stod(line.substr(regionStart + 1, colonPos));
                double rtEnd = std::stod(line.substr(colonPos + 1));
                if (rtEnd <= rtStart)
                {
                    std::cerr << "Error in line " << i << ": The lower limit of retention times (" << rtStart
                              << ") must be smaller than the right limit (" << rtEnd << ")\n";
                    exit(1);
                }
                if (rtStart < 0 || rtEnd < 0)
                {
                    std::cerr << "Error in line " << i << ": retention times cannot be negative\n";
                }

                res.back().RT_end = rtEnd;
                res.back().RT_start = rtStart;
                continue;
            }
            if (line.size() > 13 && line.substr(0, 13) == "tolerance_mz")
            {
                assert(res.back().tolerance_mz == -1);
                double tolerance = std::stod(line.substr(line.find('=')));
                if (tolerance < 0)
                {
                    std::cerr << "Error in line " << i << ": m/z tolerance cannot be negative\n";
                }
                res.back().tolerance_mz = tolerance;
                continue;
            }
            if (line.size() >= 8 && line.substr(0, 8) == "fragment")
            {
                if (mzOpen)
                {
                    std::cerr << "Error in line " << i << ": you already used the \"fragment\" command\n";
                    exit(1);
                }
                if (!res.back().fragment_mz.empty())
                {
                    std::cerr << "Error in line " << i << ": Entry no. " << res.size() << "already contains fragments\n";
                    exit(1);
                }
                mzOpen = true;
                continue;
            }
            if (line.size() >= 12 && line.substr(0, 12) == "fragment_end")
            {
                if (!mzOpen)
                {
                    std::cerr << "Error in line " << i << ": you did not use the \"fragment\" command first\n";
                    exit(1);
                }
                if (res.back().fragment_mz.empty())
                {
                    std::cerr << "Error in line " << i << ": no fragments suppied to entry no. " << res.size() << "\n";
                    exit(1);
                }
                mzOpen = false;
                continue;
            }
            if (mzOpen && line.substr(0, 1) == "=")
            {
                double mass = std::stod(line.substr(1));
                if (mass < 10)
                {
                    std::cerr << "Error in line " << i << ": masses < 10 Dalton make no sense as fragments\n";
                    exit(1);
                }
                res.back().fragment_mz.push_back(mass);
                continue;
            }
        }
        return res;
    }
}

int main(int argc, char const *argv[]) // single-file process, @todo change
{
    // read user input (relevant masses and RTs)
    std::vector<std::string> paths;
    std::string pathTargets;
    bool targetsSet = false;
    for (size_t i = 1; i < argc; i++)
    {
        std::string str = argv[i];
        size_t len = str.size();
        if (len > 8 && str.substr(len - 8) == ".targets") // string ends in ".targets"
        {
            if (targetsSet)
            {
                std::cerr << "Error: you can only have one .targets file per analysis.\n";
            }
            pathTargets = str;
            targetsSet = true;
        }
        else
        {
            paths.push_back(str);
        }
    }

    auto targets = qalgo_waters::getSearchConfig(pathTargets);

    auto tasks = qAlgorithms::controlInput(&paths, false);

    // read file using streamcraft @todo correctness check here?
    qAlgorithms::XML_File data(tasks[0]);

    // extract relevant
    std::cout << "hello world\n";
    return 0;
}
